#pragma once

#include <ciphey/typedefs.hpp>

#include <Python.h>

#include <memory>
#include <stdexcept>

namespace ciphey::swig {
  template<typename ByteContainerWithOptRef, typename Arg>
  inline void bytes_out(Arg& target, ByteContainerWithOptRef b) noexcept {
    target = PyBytes_FromStringAndSize(reinterpret_cast<char const*>(b.data()), b.size());
  }

  template<typename ByteContainer, typename Arg>
  inline void bytes_in(ByteContainer& target, Arg const& b) {
    Py_ssize_t len;
    uint8_t* ptr;
    // Does not pass ownership
    if (PyBytes_AsStringAndSize(b, reinterpret_cast<char**>(&ptr), &len) < 0)
      throw std::invalid_argument("Bad PyBytes");
    target = ByteContainer{ptr, ptr + len};
  }

  template<typename StrContainerWithOptRef, typename Arg>
  inline void str_out(Arg& target, StrContainerWithOptRef b) noexcept {
    target = PyUnicode_DecodeUTF8(b.data(), b.size(), nullptr);
  }

  template<typename StrContainer, typename Arg>
  inline void str_in(StrContainer& target, Arg const& b) {
    Py_ssize_t len;
    const char* ptr;
    // Does not pass ownership
    if ((ptr = PyUnicode_AsUTF8AndSize(b, &len)) == nullptr)
      throw std::invalid_argument("Bad PyUnicode");
    target = StrContainer{ptr, ptr + len};
  }
}
