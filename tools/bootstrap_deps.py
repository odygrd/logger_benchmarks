#!/usr/bin/env python3
"""Restore benchmark dependencies from pinned sources."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


def run_step(cmd: list[str]) -> None:
    print(f"[run] {' '.join(cmd)}")
    subprocess.run(cmd, cwd=repo_root(), check=True)


def main() -> int:
    parser = argparse.ArgumentParser(description="Restore all benchmark dependencies.")
    parser.add_argument(
        "--skip-boost",
        action="store_true",
        help="Skip the Boost.Log bootstrap step and only sync git-based vendor trees.",
    )
    args = parser.parse_args()

    python = sys.executable
    run_step([python, "tools/update_vendor.py", "sync", "--all"])

    if not args.skip_boost:
        run_step([python, "tools/setup_boost_simple.py"])

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
