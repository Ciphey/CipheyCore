#pragma once

#include <ciphey/typedefs.hpp>

#include <Python.h>

#include <memory>

namespace ciphey::swig {
  template<typename Arg>
  inline void bytes_out(Arg& target, data const& b) noexcept {
    target = PyBytes_FromStringAndSize(reinterpret_cast<char const*>(b.data()), b.size());
  }

  template<typename Arg>
  inline void bytes_in(data& target, Arg const& b) {
    Py_ssize_t len;
    char* ptr;
    // Does not pass ownership
    if (PyBytes_AsStringAndSize(b, &ptr, &len) < 0)
      throw std::invalid_argument("Bad PyBytes");
    target = data{ptr, ptr + len};
  }
}
