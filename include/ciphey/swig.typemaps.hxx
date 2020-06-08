#pragma once

#include <ciphey/typedefs.hpp>

#include <Python.h>

#include <memory>

namespace ciphey::swig {
  template<typename Arg>
  inline void bytes_in(Arg& target, bytes_t const& b) noexcept {
    target = PyBytes_FromStringAndSize(reinterpret_cast<char const*>(b.data()), b.size());
  }

  template<typename Arg>
  inline void bytes_out(bytes_t& target, Arg const& b) {
    size_t len;
    std::unique_ptr<char> str;
    {
      char* ptr;
      if (!PyBytes_AsStringAndSize(b, &ptr, len))
        throw std::invalid_argument("Bad PyBytes");
      str.reset(ptr);
    }
    target = bytes_t{str.data(), str.data() + len};
  }
}
