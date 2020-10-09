![PyPI](https://img.shields.io/pypi/v/cipheycore)
![Tests](https://github.com/Ciphey/CipheyCore/workflows/Tests/badge.svg)
[![Downloads](https://pepy.tech/badge/cipheycore)](https://pepy.tech/project/cipheycore)
[![Downloads](https://pepy.tech/badge/cipheycore/month)](https://pepy.tech/project/cipheycore/month)
[![Downloads](https://pepy.tech/badge/cipheycore/week)](https://pepy.tech/project/cipheycore/week)
# CipheyCore
Some cryptanalysis tidbits written in a *proper* language

This core provides many crackers, encryption methods & decryption methods as well as cryptanalysis tidbitis to Ciphey. Written in C++ for the speed.

## Dependencies
CipheyCore builds as a static library, but depends on some of Boost's header-only libraries.

| Platform | Command |
| - | - | 
| Debian/Ubuntu/Mint | `apt-get install libboost-dev cmake build-essential swig` |
| Arch | `pacman -S boost gcc cmake make swig` |
| CentOS/RHEL | `yum install boost-devel cmake3 llvm-toolchain-7 make swig` |
| Homebrew (OSX) | `brew install boost swig` |
| Chocolatey (Windows) | `choco install swig` and install boost from boost.org |

:warning: **Note:** This project requires `gcc>=8`. Depending on which platform you are on, you might experience that the default gcc is a lower version. You can check your gcc version by running `gcc -v`.
## Building

CipheyCore can be used as a C++ library for use in other C++ projects, or as a python3 module (with reduced functionality and speed). 

The python module depends on the C++ library, so follow both instructions if you want the python module.

### C++ library
#### Linux/OSX/other Unices
```bash
git clone https://github.com/Ciphey/CipheyCore
cd CipheyCore
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCIPHEY_CORE_TEST=OFF # Or '-DCMAKE_BUILD_TYPE=Debug' for a debug build
cmake --build . -t ciphey_core
```
If that doesn't work, try setting `-DCIPHEY_CORE_PYTHON=` to the folder containing your python headers and `-DBOOST_ROOT=` to the directory containing your boost headers.

#### Windows
```cmd
git clone https://github.com/Ciphey/CipheyCore
cd CipheyCore
rd /S build
cd build
cmake .. -DCIPHEY_CORE_TEST=OFF
cmake --build . -t ciphey_core --config Release # Or '--config Debug' for a debug build
poetry build
```

As with the Unix compilation, if you are having problems with missing files, try setting `-DCIPHEY_CORE_PYTHON` and `BOOST_ROOT`.

### Python3 library
This requires python-poetry, which can be installed with `python3 -m pip install poetry` (or, failing that, `python -m pip install poetry`).

After you have built the C++ library, compile the python interface with
```
cmake --build . -t ciphey_core_py --config Release # Or '--config Debug' for a debug build
poetry build
```

The wheel will be located in the `dist` folder. The source distribution (`.tar.gz` file) produced is rather useless, as it contains the compiled platform-specific library.
