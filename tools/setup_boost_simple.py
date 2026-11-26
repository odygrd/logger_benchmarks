#!/usr/bin/env python3
"""
Download Boost and build Boost.Log and its dependencies.
"""

import sys
import subprocess
import shutil
import tarfile
import tempfile
from pathlib import Path
from urllib.request import urlretrieve


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


# Configuration
BOOST_VERSION = "1.92.0"
BOOST_URL = f"https://github.com/boostorg/boost/releases/download/boost-{BOOST_VERSION}/boost-{BOOST_VERSION}-cmake.tar.gz"
TARGET_DIR = repo_root() / "third_party" / "boost"


def download_and_extract_boost(temp_dir):
    """Download and extract Boost."""
    print(f"Downloading Boost {BOOST_VERSION}...")
    boost_archive = temp_dir / f"boost-{BOOST_VERSION}-cmake.tar.gz"

    urlretrieve(BOOST_URL, boost_archive)
    print(f"Downloaded to {boost_archive}")

    print("Extracting Boost archive...")
    with tarfile.open(boost_archive, "r:gz") as tar:
        tar.extractall(temp_dir)

    boost_root = temp_dir / f"boost-{BOOST_VERSION}"
    return boost_root


def build_boost_log(boost_root, install_dir):
    """Build and install Boost.Log and its dependencies."""
    build_dir = boost_root / "build"
    print(f"Building Boost.Log to {install_dir}...")
    subprocess.run([
        "cmake", "-S", str(boost_root), "-B", str(build_dir),
        "-DCMAKE_BUILD_TYPE=Release",
        f"-DCMAKE_INSTALL_PREFIX={install_dir}",
        "-DCMAKE_INSTALL_LIBDIR=lib",
        "-DBOOST_INCLUDE_LIBRARIES=log",
        "-DBUILD_SHARED_LIBS=OFF",
        "-DBUILD_TESTING=OFF",
    ], check=True)
    subprocess.run(["cmake", "--build", str(build_dir), "--parallel", "4"], check=True)
    subprocess.run(["cmake", "--install", str(build_dir)], check=True)

    print("Boost.Log built successfully!")


def create_cmake_file(install_dir):
    """Create CMakeLists.txt for Boost.Log."""
    print("Creating CMakeLists.txt...")

    cmake_content = '''# Boost.Log library - using prebuilt Boost
# Libraries built by tools/setup_boost_simple.py

find_package(Boost CONFIG REQUIRED COMPONENTS log_setup
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake"
    NO_DEFAULT_PATH
)

add_library(boost_log INTERFACE)
target_link_libraries(boost_log INTERFACE Boost::log_setup)
'''

    cmake_file = install_dir / "CMakeLists.txt"
    cmake_file.write_text(cmake_content)
    print(f"Created {cmake_file}")


def main():
    """Main execution function."""
    print("=" * 60)
    print("Boost.Log Simple Setup Script")
    print("=" * 60)

    # Clean existing directory if it exists
    if TARGET_DIR.exists():
        print(f"Removing existing {TARGET_DIR}...")
        shutil.rmtree(TARGET_DIR)

    # Create target directory
    TARGET_DIR.mkdir(parents=True)

    # Create temporary directory
    with tempfile.TemporaryDirectory() as temp_dir_str:
        temp_dir = Path(temp_dir_str)
        print(f"Using temporary directory: {temp_dir}")

        try:
            # Download and extract Boost
            boost_root = download_and_extract_boost(temp_dir)

            # Build and install Boost.Log
            build_boost_log(boost_root, TARGET_DIR)

            # Create CMakeLists.txt
            create_cmake_file(TARGET_DIR)

            print("=" * 60)
            print("SUCCESS!")
            print("=" * 60)
            print(f"Boost has been installed to: {TARGET_DIR}")
            print("\nNext steps:")
            print("1. Configure the project with CMake")
            print("2. Build the benchmark targets")

        except Exception as e:
            print(f"ERROR: {e}", file=sys.stderr)
            import traceback
            traceback.print_exc()
            sys.exit(1)


if __name__ == "__main__":
    main()
