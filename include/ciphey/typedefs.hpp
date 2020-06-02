// Swig complains if this is set
//#pragma once

#include <map>
#include <vector>
#include <string>

namespace ciphey {
  using char_t = char;
  using float_t = float;
  using prob_t = float_t;
  using freq_t = size_t;

  using string_t = std::basic_string<char_t>;

  using freq_table = std::map<char_t, freq_t>;
  using windowed_freq_table = std::vector<freq_table>;
  using prob_table = std::map<char_t, prob_t>;
  using windowed_prob_table = std::vector<prob_table>;

  using group_t = std::vector<char_t>;
  using inverse_group_t = std::map<char_t, size_t>;

  namespace caesar {
    using key_t = size_t;
  }

  namespace vigenere {
    /// A sequence of offsets
    using key_t = std::vector<size_t>;
  }
}
