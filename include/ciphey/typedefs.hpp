// Swig complains if this is set
//#pragma once

#include <map>
#include <vector>
#include <string>

namespace ciphey {
  typedef char char_t;
  typedef float float_t;
  typedef float_t prob_t;
  typedef size_t freq_t;

  typedef std::basic_string<char_t> string_t;

  typedef std::map<char_t, freq_t> freq_table;
  typedef std::vector<freq_table> windowed_freq_table;
  typedef std::map<char_t, prob_t> prob_table;
  typedef std::vector<prob_table> windowed_prob_table;

  typedef std::vector<char_t> group_t;
  typedef std::map<char_t, size_t> inverse_group_t;

  namespace caesar {
    typedef size_t key_t;
  }
}
