// Swig complains if this is set
//#pragma once

#include <map>
#include <vector>
#include <string>
#include <set>
#include <ciphey/span.hpp>

namespace ciphey {
  using char_t = char;
  using float_t = double;
  using prob_t = float_t;
  using freq_t = size_t;

  using string_t = std::basic_string<char_t>;
  using string_ref_t = nonstd::span<char_t>;
  // Not using string_view because casting is painful
  using string_const_ref_t = nonstd::span<const char>;

  using bytes_t = std::vector<uint8_t>;
  using bytes_ref_t = nonstd::span<uint8_t>;
  using bytes_const_ref_t = nonstd::span<const uint8_t>;

  using domain_t = std::set<char_t>;
  using freq_table = std::map<char_t, freq_t>;
  using windowed_freq_table = std::vector<freq_table>;
  using prob_table = std::map<char_t, prob_t>;
  using windowed_prob_table = std::vector<prob_table>;
}
