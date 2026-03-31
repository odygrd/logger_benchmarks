#!/usr/bin/env python3
"""Sync vendored third-party source trees from pinned upstream refs."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Dependency:
    name: str
    repo_url: str
    ref: str
    target_dir: Path
    skip_update: bool = False
    update_branch: str | None = None


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


def manifest_path() -> Path:
    return repo_root() / "vendor_manifest.json"


def cache_root() -> Path:
    return repo_root() / ".vendor-cache"


def patches_root() -> Path:
    return repo_root() / "vendor_patches"


def run(cmd: list[str], *, cwd: Path | None = None, capture_output: bool = False) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        cmd,
        cwd=cwd,
        check=True,
        text=True,
        capture_output=capture_output,
    )


def load_manifest() -> tuple[dict, list[Dependency]]:
    raw_manifest = json.loads(manifest_path().read_text(encoding="utf-8"))
    dependencies = [
        Dependency(
            name=entry["name"],
            repo_url=entry["repo_url"],
            ref=entry["ref"],
            target_dir=Path(entry["target_dir"]),
            skip_update=entry.get("skip_update", False),
            update_branch=entry.get("update_branch"),
        )
        for entry in raw_manifest["dependencies"]
    ]
    return raw_manifest, dependencies


def write_manifest(raw_manifest: dict) -> None:
    manifest_path().write_text(json.dumps(raw_manifest, indent=2) + "\n", encoding="utf-8")


def ensure_repo_cache(dep: Dependency) -> Path:
    cache_dir = cache_root() / dep.name
    cache_dir.parent.mkdir(parents=True, exist_ok=True)

    if not cache_dir.exists():
        print(f"[clone] {dep.name}: {dep.repo_url}")
        run(["git", "clone", "--filter=blob:none", "--no-checkout", dep.repo_url, str(cache_dir)])
    else:
        run(["git", "-C", str(cache_dir), "remote", "set-url", "origin", dep.repo_url])

    print(f"[fetch] {dep.name}")
    run(["git", "-C", str(cache_dir), "fetch", "--force", "--tags", "origin"])
    return cache_dir


def resolve_ref(dep: Dependency, override_ref: str | None = None) -> str:
    cache_dir = ensure_repo_cache(dep)
    ref = override_ref or dep.ref
    result = run(
        ["git", "-C", str(cache_dir), "rev-parse", f"{ref}^{{commit}}"],
        capture_output=True,
    )
    return result.stdout.strip()


def export_ref(cache_dir: Path, commit: str, destination: Path) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    archive = subprocess.Popen(
        ["git", "-C", str(cache_dir), "archive", commit],
        stdout=subprocess.PIPE,
    )
    try:
        extract = subprocess.run(
            ["tar", "-x", "-C", str(destination)],
            stdin=archive.stdout,
            check=True,
        )
        _ = extract
    finally:
        if archive.stdout is not None:
            archive.stdout.close()
        archive_return = archive.wait()
        if archive_return != 0:
            raise subprocess.CalledProcessError(archive_return, archive.args)


def normalize_line_endings(data: bytes) -> bytes:
    return data.replace(b"\r\n", b"\n").replace(b"\r", b"\n")


def query_head_file(pathspec: str) -> bytes:
    return subprocess.check_output(["git", "show", f"HEAD:{pathspec}"], cwd=repo_root())


def tracked_eol_info(pathspec: Path) -> dict[str, str]:
    result = subprocess.run(
        ["git", "-c", "core.quotePath=false", "ls-files", "--eol", "-z", "--", str(pathspec)],
        cwd=repo_root(),
        check=True,
        capture_output=True,
    )

    info: dict[str, str] = {}
    for entry in result.stdout.split(b"\0"):
        if not entry:
            continue

        metadata_bytes, repo_path_bytes = entry.split(b"\t", 1)
        metadata = metadata_bytes.decode("utf-8", "surrogateescape")
        repo_path = repo_path_bytes.decode("utf-8", "surrogateescape")
        index_token = metadata.split()[0]
        if not index_token.startswith("i/"):
            continue
        info[repo_path] = index_token.split("/", 1)[1]

    return info


def convert_line_endings(data: bytes, style: str) -> bytes:
    normalized = normalize_line_endings(data)
    if style == "crlf":
        return normalized.replace(b"\n", b"\r\n")
    if style == "lf":
        return normalized
    return data


def preserve_tracked_file_format(dep: Dependency, destination: Path) -> None:
    for repo_path, index_eol in tracked_eol_info(dep.target_dir).items():
        try:
            relative_path = Path(repo_path).relative_to(dep.target_dir)
        except ValueError:
            continue
        staged_path = destination / relative_path

        if not staged_path.is_file():
            continue

        staged_bytes = staged_path.read_bytes()
        if b"\0" in staged_bytes:
            continue

        if index_eol in {"lf", "crlf"}:
            converted = convert_line_endings(staged_bytes, index_eol)
            if converted != staged_bytes:
                staged_path.write_bytes(converted)
            continue

        if index_eol != "mixed":
            continue

        head_bytes = query_head_file(repo_path)
        if normalize_line_endings(head_bytes) == normalize_line_endings(staged_bytes):
            staged_path.write_bytes(head_bytes)


def apply_patches(dep: Dependency, destination: Path) -> int:
    dep_patch_dir = patches_root() / dep.name
    if not dep_patch_dir.exists():
        return 0

    patches = sorted(dep_patch_dir.glob("*.patch"))
    destination_arg = str(destination.relative_to(repo_root()))
    for patch in patches:
        print(f"[patch] {dep.name}: {patch.name}")
        run(
            ["git", "apply", "--whitespace=nowarn", "-p1", f"--directory={destination_arg}", str(patch)],
            cwd=repo_root(),
        )

    return len(patches)


def stage_dependency(dep: Dependency) -> None:
    paths = [str(manifest_path()), str(dep.target_dir)]
    run(["git", "add", "-A", "--", *paths], cwd=repo_root())
    print(f"[stage] {dep.name}")


def remove_tree_contents(path: Path) -> None:
    for child in path.iterdir():
        if child.name == ".git":
            continue
        if child.is_dir() and not child.is_symlink():
            shutil.rmtree(child)
        else:
            child.unlink()


def copy_tree_contents(source: Path, destination: Path) -> None:
    destination.mkdir(parents=True, exist_ok=True)
    for child in source.iterdir():
        target = destination / child.name
        if child.is_dir() and not child.is_symlink():
            shutil.copytree(child, target, symlinks=True)
        else:
            shutil.copy2(child, target, follow_symlinks=False)


def capture_patch(dep: Dependency, commit: str) -> Path | None:
    target_dir = repo_root() / dep.target_dir
    if not target_dir.exists():
        raise SystemExit(f"cannot capture patch for missing target directory: {dep.target_dir}")

    cache_dir = cache_root() / dep.name
    patch_dir = patches_root() / dep.name
    patch_dir.mkdir(parents=True, exist_ok=True)
    patch_path = patch_dir / "0001-local.patch"

    with tempfile.TemporaryDirectory(prefix=f"{dep.name}-capture-", dir=repo_root()) as temp_dir:
        work_dir = Path(temp_dir) / "work"
        export_ref(cache_dir, commit, work_dir)

        run(["git", "init"], cwd=work_dir, capture_output=True)
        run(["git", "config", "user.name", "vendor-sync"], cwd=work_dir)
        run(["git", "config", "user.email", "vendor-sync@example.invalid"], cwd=work_dir)
        run(["git", "add", "-A"], cwd=work_dir)
        run(["git", "commit", "-m", "base"], cwd=work_dir, capture_output=True)

        remove_tree_contents(work_dir)
        copy_tree_contents(target_dir, work_dir)
        run(["git", "add", "-A"], cwd=work_dir)

        diff = subprocess.run(
            [
                "git",
                "diff",
                "--cached",
                "--binary",
                "--full-index",
                "--no-ext-diff",
                "--src-prefix=a/",
                "--dst-prefix=b/",
                "HEAD",
            ],
            cwd=work_dir,
            check=False,
            text=True,
            capture_output=True,
        )

    for existing_patch in patch_dir.glob("*.patch"):
        existing_patch.unlink()

    if diff.stdout:
        patch_path.write_text(diff.stdout, encoding="utf-8")
        return patch_path

    return None


def sync_dependency(dep: Dependency, commit: str, *, apply_local_patches: bool = True) -> None:
    root = repo_root()
    target_dir = (root / dep.target_dir).resolve()
    if root not in target_dir.parents and target_dir != root:
        raise RuntimeError(f"refusing to write outside repo root: {target_dir}")

    for stale_staging_dir in target_dir.parent.glob(f".{target_dir.name}.vendor-staging-*"):
        if stale_staging_dir.is_dir():
            shutil.rmtree(stale_staging_dir)

    staging_dir = target_dir.parent / f".{target_dir.name}.vendor-staging-{os.getpid()}"
    if staging_dir.exists():
        shutil.rmtree(staging_dir)

    cache_dir = cache_root() / dep.name
    print(f"[export] {dep.name}: {commit} -> {dep.target_dir}")
    export_ref(cache_dir, commit, staging_dir)
    patch_count = apply_patches(dep, staging_dir) if apply_local_patches else 0
    preserve_tracked_file_format(dep, staging_dir)
    if patch_count == 0 and target_dir.exists() and apply_local_patches:
        print(
            f"[warning] {dep.name}: no local patch files recorded; sync will replace {dep.target_dir} with the raw upstream snapshot",
            file=sys.stderr,
        )

    if target_dir.exists():
        print(f"[replace] removing {dep.target_dir}")
        shutil.rmtree(target_dir)

    staging_dir.rename(target_dir)


def select_dependencies(all_dependencies: list[Dependency], names: list[str], all_flag: bool) -> list[Dependency]:
    if all_flag:
        if names:
            raise SystemExit("cannot combine --all with explicit dependency names")
        return all_dependencies

    if not names:
        raise SystemExit("select at least one dependency name or pass --all")

    selected = []
    dependency_map = {dep.name: dep for dep in all_dependencies}
    missing = [name for name in names if name not in dependency_map]
    if missing:
        raise SystemExit(f"unknown dependencies: {', '.join(sorted(missing))}")

    for name in names:
        selected.append(dependency_map[name])

    return selected


def cmd_list(all_dependencies: list[Dependency]) -> int:
    for dep in all_dependencies:
        suffix_parts = []
        if dep.skip_update:
            suffix_parts.append("skip-update")
        if dep.update_branch:
            suffix_parts.append(f"update-branch={dep.update_branch}")
        suffix = f" [{' '.join(suffix_parts)}]" if suffix_parts else ""
        print(f"{dep.name:20} {dep.ref} {dep.target_dir}{suffix}")
    return 0


def cmd_resolve(args: argparse.Namespace, raw_manifest: dict, all_dependencies: list[Dependency]) -> int:
    _ = raw_manifest
    selected = select_dependencies(all_dependencies, args.names, args.all)
    for dep in selected:
        resolved = resolve_ref(dep)
        print(f"{dep.name}: {dep.ref} -> {resolved}")
    return 0


def update_manifest_ref(raw_manifest: dict, dep_name: str, resolved_commit: str) -> None:
    for entry in raw_manifest["dependencies"]:
        if entry["name"] == dep_name:
            entry["ref"] = resolved_commit
            return
    raise RuntimeError(f"dependency not found in manifest: {dep_name}")


def resolve_remote_tip(dep: Dependency, branch: str) -> tuple[str, str]:
    cache_dir = ensure_repo_cache(dep)
    candidates = [f"refs/remotes/origin/{branch}", "refs/remotes/origin/HEAD"]

    for candidate in candidates:
        result = subprocess.run(
            ["git", "-C", str(cache_dir), "rev-parse", f"{candidate}^{{commit}}"],
            check=False,
            text=True,
            capture_output=True,
        )
        if result.returncode == 0:
            return result.stdout.strip(), candidate

    raise RuntimeError(f"could not resolve remote tip for {dep.name}")


def cmd_sync(args: argparse.Namespace, raw_manifest: dict, all_dependencies: list[Dependency]) -> int:
    selected = select_dependencies(all_dependencies, args.names, args.all)

    if args.ref and (args.all or len(selected) != 1):
        raise SystemExit("--ref can only be used when syncing exactly one dependency")
    if args.ref and args.update:
        raise SystemExit("--ref cannot be combined with --update")

    for dep in selected:
        if args.update:
            if dep.skip_update:
                resolved = resolve_ref(dep)
                print(f"[pinned] {dep.name}: staying on {dep.ref} -> {resolved}")
            else:
                preferred_branch = dep.update_branch or args.branch
                resolved, source_ref = resolve_remote_tip(dep, preferred_branch)
                print(f"[updated] {dep.name}: {source_ref} -> {resolved}")
                dep = Dependency(
                    dep.name, dep.repo_url, resolved, dep.target_dir, dep.skip_update, dep.update_branch
                )
        else:
            resolved = resolve_ref(dep, args.ref)
            print(f"[resolved] {dep.name}: {(args.ref or dep.ref)} -> {resolved}")

        if args.ref:
            dep = Dependency(
                dep.name, dep.repo_url, resolved, dep.target_dir, dep.skip_update, dep.update_branch
            )

        sync_dependency(dep, resolved, apply_local_patches=not args.update)

        if args.ref or (args.update and not dep.skip_update):
            update_manifest_ref(raw_manifest, dep.name, resolved)
            write_manifest(raw_manifest)

        if args.update:
            stage_dependency(dep)

    return 0


def cmd_capture_patch(args: argparse.Namespace, raw_manifest: dict, all_dependencies: list[Dependency]) -> int:
    _ = raw_manifest
    selected = select_dependencies(all_dependencies, args.names, args.all)

    for dep in selected:
        resolved = resolve_ref(dep)
        patch_path = capture_patch(dep, resolved)
        if patch_path is None:
            print(f"[captured] {dep.name}: no local patch needed")
        else:
            print(f"[captured] {dep.name}: {patch_path.relative_to(repo_root())}")

    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="Manage vendored third-party dependencies.")
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("list", help="List manifest entries.")

    resolve_parser = subparsers.add_parser("resolve", help="Resolve pinned refs to exact commits.")
    resolve_parser.add_argument("names", nargs="*", help="Dependency names to resolve.")
    resolve_parser.add_argument("--all", action="store_true", help="Resolve every dependency in the manifest.")

    sync_parser = subparsers.add_parser("sync", help="Replace vendored directories from pinned refs.")
    sync_parser.add_argument("names", nargs="*", help="Dependency names to sync.")
    sync_parser.add_argument("--all", action="store_true", help="Sync every dependency in the manifest.")
    sync_parser.add_argument(
        "--ref",
        help="Override the manifest ref for a single dependency, resolve it to a commit, and write it back.",
    )
    sync_parser.add_argument(
        "-update",
        "--update",
        action="store_true",
        help="Resolve each selected dependency to the latest remote branch tip, except manifest entries marked with skip_update, sync it with the existing local patches, write it to the manifest, and stage the affected files.",
    )
    sync_parser.add_argument(
        "--branch",
        default="master",
        help="Preferred remote branch name for --update. Falls back to the remote default branch.",
    )

    capture_parser = subparsers.add_parser(
        "capture-patch",
        help="Capture the current local delta for vendored directories into vendor_patches/<name>/0001-local.patch.",
    )
    capture_parser.add_argument("names", nargs="*", help="Dependency names to capture.")
    capture_parser.add_argument("--all", action="store_true", help="Capture patches for every dependency in the manifest.")

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    raw_manifest, all_dependencies = load_manifest()

    try:
        if args.command == "list":
            return cmd_list(all_dependencies)
        if args.command == "resolve":
            return cmd_resolve(args, raw_manifest, all_dependencies)
        if args.command == "sync":
            return cmd_sync(args, raw_manifest, all_dependencies)
        if args.command == "capture-patch":
            return cmd_capture_patch(args, raw_manifest, all_dependencies)
    except subprocess.CalledProcessError as exc:
        print(f"command failed: {' '.join(exc.cmd)}", file=sys.stderr)
        return exc.returncode

    parser.error(f"unknown command: {args.command}")
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
