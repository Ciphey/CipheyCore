import ctypes
import numpy
import glob

# find the shared library, the path depends on the platform and Python version
libfile = glob.glob('build/*/gof_test*.so')[0]

# 1. open the shared library
helper = ctypes.CDLL(libfile)

# 2. tell Python the argument and result types of function mysum
helper.gof_test.restype = ctypes.c_double
helper.gof_test.argtypes = [ctypes.pointer.__dict__[str, float], 
                        numpy.ctypeslib.ndpointer(dtype=numpy.int32)]

