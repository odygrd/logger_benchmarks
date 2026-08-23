# setup.py — forces platform-tagged abi3 wheel for Stable ABI
# The package contains a pre-built C extension compiled with Py_LIMITED_API.
from setuptools import setup, Distribution
from wheel.bdist_wheel import bdist_wheel


class BinaryDistribution(Distribution):
    """Mark the distribution as platform-specific."""
    def has_ext_modules(self):
        return True


class bdist_wheel_abi3(bdist_wheel):
    """Build wheel tagged for Stable ABI (abi3) targeting Python 3.7+."""
    def get_tag(self):
        python, abi, plat = super().get_tag()
        return "cp37", "abi3", plat


setup(
    distclass=BinaryDistribution,
    cmdclass={"bdist_wheel": bdist_wheel_abi3},
)
