from setuptools import setup, Extension

# Compile parts of `swig.hpp` into a shared library so we can call it from Python
setup(
    #...
    ext_modules=[Extension('gof_test', ['freq.cpp'],),],
)