#pragma once

#include <Ciphey/freq.hpp>
#include <Ciphey/typedefs.hpp>

#include <string>
#include <vector>

#include <cstdint>

namespace Ciphey {
  std::map<size_t, char_t> invert_group(group_t const&);

  template<typename Key>
  struct crack_results {
    Key key;
    prob_t p_value;
  };

  namespace caesar {
    void crypt(std::u16string& str, key_t const& key, group_t const& group);

    // If the p_value is set to one, we will keep going to try to maximise the p-value
    crack_results<key_t> crack(prob_table observed, prob_table const& expected, group_t const& group, freq_t count,
                               prob_t p_value = 1);
  }

  namespace vigenere {
    /// A sequence of offsets
    using key_t = std::vector<size_t>;
    void crypt(std::u16string& str, key_t const& key, std::vector<char_t> const& group, bool ignore_missing = true);

    crack_results<key_t> crack(windowed_prob_table observed, prob_table const& expected, group_t const& group, freq_t count, prob_t p_value);
  }
}
