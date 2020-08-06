%module cipheycore

%ignore std::vector<ciphey::ausearch_edge>::vector(size_type);
%ignore std::vector<ciphey::ausearch_edge>::resize;


%include std_basic_string.i
%include std_string.i
%include std_map.i
%include std_vector.i
%include std_shared_ptr.i
%include std_set.i
%include stdint.i
%include std_vector.i

namespace ciphey {
  %typemap(in)  bytes_t {
    Py_ssize_t len;
    uint8_t* ptr;
    // Does not pass ownership
    if (PyBytes_AsStringAndSize($input, reinterpret_cast<char**>(&ptr), &len) < 0)
      throw std::invalid_argument("Bad PyBytes");

    $1 = ::ciphey::bytes_t{ptr, ptr + len};
  }
  %typemap(out) bytes_t           {
    $result = PyBytes_FromStringAndSize(reinterpret_cast<char const*>($1.data()), $1.size());
  }

  %typemap(in)  bytes_const_ref_t { ::ciphey::swig::bytes_in<::ciphey::bytes_const_ref_t>($1, $input); }
  %typemap(out) bytes_const_ref_t { ::ciphey::swig::bytes_out<::ciphey::bytes_const_ref_t>($result, $1); }

  %typemap(typecheck,precedence=SWIG_TYPECHECK_INT8_ARRAY) bytes_t { $1 = PyBytes_Check($input); }
  %typemap(typecheck,precedence=SWIG_TYPECHECK_INT8_ARRAY) bytes_const_ref_t { $1 = PyBytes_Check($input); }


  %typemap(in)  string_const_ref_t { ::ciphey::swig::str_in<::ciphey::string_const_ref_t>($1, $input); }
  %typemap(out) string_const_ref_t { ::ciphey::swig::str_out<::ciphey::string_const_ref_t>($result, $1); }

  %typemap(typecheck,precedence=SWIG_TYPECHECK_STRING) string_const_ref_t { $1 = PyUnicode_Check($input); }
}

%include "ciphey/swig.typemaps.hxx"

%{
  #include <map>
  #define SWIG_FILE_WITH_INIT
  #include "ciphey/typedefs.hpp"
  #include "ciphey/swig.typemaps.hxx"
  #include "ciphey/swig.hpp"
%}

%shared_ptr(ciphey::simple_analysis_res)
%shared_ptr(ciphey::windowed_analysis_res)

%include "ciphey/typedefs.hpp"

namespace std {
  %template(freq_table) map<ciphey::char_t, ciphey::freq_t>;
  %template(prob_table) map<ciphey::char_t, ciphey::prob_t>;
  %template(group_t) vector<ciphey::char_t>;
  %template(viginere_key_t) vector<size_t>;
  %template(domain_t) set<ciphey::char_t>;
  %template(ausearch_edges_t) vector<ciphey::ausearch_edge>;
  %template(bytes_t) vector<uint8_t>;
}

%include "ciphey/ausearch.swig.hxx"

%include "ciphey/ciphers.swig.hxx"
%include "ciphey/swig.hpp"

namespace ciphey {
  %template(caesar_results_elem) crack_result<ciphey::caesar::key_t>;
  %template(vigenere_results_elem) crack_result<ciphey::vigenere::key_t>;
  %template(xor_single_results_elem) crack_result<ciphey::xor_single::key_t>;
  %template(xorcrypt_results_elem) crack_result<ciphey::xorcrypt::key_t>;
}
namespace std {
  %template(caesar_results) vector<ciphey::crack_result<ciphey::caesar::key_t>>;
  %template(vigenere_results) vector<ciphey::crack_result<ciphey::vigenere::key_t>>;
  %template(xor_single_results) vector<ciphey::crack_result<ciphey::xor_single::key_t>>;
  %template(xorcrypt_results) vector<ciphey::crack_result<ciphey::xorcrypt::key_t>>;

  %template(vigenere_key_len_candidates) vector<ciphey::vigenere_key_len_candidate>;
}
