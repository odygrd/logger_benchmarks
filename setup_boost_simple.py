#!/usr/bin/env python3
"""
Simple script to download Boost and build only Boost.Log using b2.
"""

import os
import sys
import subprocess
import shutil
import tarfile
import tempfile
from pathlib import Path
from urllib.request import urlretrieve

# Configuration
BOOST_VERSION = "1.89.0"
BOOST_VERSION_UNDERSCORE = BOOST_VERSION.replace(".", "_")
BOOST_URL = f"https://github.com/boostorg/boost/releases/download/boost-{BOOST_VERSION}/boost-{BOOST_VERSION}-b2-nodocs.tar.gz"
TARGET_DIR = Path(__file__).parent / "third_party" / "boost"


def download_and_extract_boost(temp_dir):
    """Download and extract Boost."""
    print(f"Downloading Boost {BOOST_VERSION}...")
    boost_archive = temp_dir / f"boost-{BOOST_VERSION}-b2-nodocs.tar.gz"

    urlretrieve(BOOST_URL, boost_archive)
    print(f"Downloaded to {boost_archive}")

    print("Extracting Boost archive...")
    with tarfile.open(boost_archive, "r:gz") as tar:
        tar.extractall(temp_dir)

    boost_root = temp_dir / f"boost-{BOOST_VERSION}"
    return boost_root


def build_boost_log(boost_root, install_dir):
    """Build Boost.Log using b2."""
    print("Bootstrapping Boost.Build...")
    subprocess.run([str(boost_root / "bootstrap.sh")], cwd=boost_root, check=True)

    print(f"Building Boost.Log to {install_dir}...")
    # Build only log library and its dependencies
    subprocess.run([
        str(boost_root / "b2"),
        "--with-log",  # Only build log
        f"--prefix={install_dir}",
        "variant=release",
        "link=static",
        "threading=multi",
        "runtime-link=shared",
        "-j4",
        "install"
    ], cwd=boost_root, check=True)

    print("Boost.Log built successfully!")


def create_cmake_file(install_dir):
    """Create CMakeLists.txt for Boost.Log."""
    print("Creating CMakeLists.txt...")

    cmake_content = '''# Boost.Log library - using prebuilt Boost
# Libraries built by setup_boost_simple.py using Boost's b2 build system

# Create interface library for Boost.Log
add_library(boost_log INTERFACE)

# Set include directory
target_include_directories(boost_log INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link all required Boost libraries
target_link_libraries(boost_log INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libboost_log.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libboost_log_setup.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libboost_thread.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libboost_filesystem.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libboost_chrono.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libboost_regex.a
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libboost_atomic.a
    Threads::Threads
    rt
)
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
            print("1. Update CMakeLists.txt to add: add_subdirectory(third_party/boost)")
            print("2. Link against 'boost_log' in your benchmark targets")

        except Exception as e:
            print(f"ERROR: {e}", file=sys.stderr)
            import traceback
            traceback.print_exc()
            sys.exit(1)


if __name__ == "__main__":
    main()
