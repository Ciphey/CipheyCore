#pragma once

#include <Ciphey/freq.hpp>
#include <Ciphey/typedefs.hpp>

#include <string>
#include <vector>

#include <cstdint>

namespace Ciphey {
  std::map<size_t, char_t> invert_group(group_t const&);

  namespace caesar {
    void crypt(std::u16string& str, key_t const& key, group_t const& group);

    key_t crack(prob_table observed, prob_table const& expected, group_t const& group, freq_t count, prob_t p_value);
  }

  namespace vigenere {
    // A sequence of offsets
    using key_t = std::vector<size_t>;
    void crypt(std::u16string& str, key_t const& key, std::vector<char_t> const& group, bool ignore_missing = true);

    struct crack_context {
      // The group in which all chars lie, in order
      group_t group;
      // The inverse map char => pos in group
//      std::map<char_t, size_t> key;
      // The previous candidate
      key_t prev_key;
      // The length of the plaintext
      size_t str_len;
      // The expected probabilities
      prob_table tab;
      // The minimum p-value
      prob_t p_value;
    };

    key_t crack(crack_context const&);
  }
}
