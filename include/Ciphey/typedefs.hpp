// Swig complains if this is set
//#pragma once

#include <map>
#include <vector>
#include <string>

namespace Ciphey {
  using char_t = char;
  using float_t = float;
  using prob_t = float_t;
  using freq_t = size_t;

  using string_t = std::basic_string<char_t>;

  using freq_table = std::map<char_t, freq_t>;
  using prob_table = std::map<char_t, prob_t>;

  using group_t = std::vector<char_t>;

  namespace caesar {
    using key_t = size_t;
  }
}
