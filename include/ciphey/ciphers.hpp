#pragma once

#include <ciphey/freq.hpp>
#include <ciphey/typedefs.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include <cstdint>

#include <ciphey/ciphers.swig.hxx>

namespace ciphey {
  constexpr prob_t default_p_value = 1e-3f;

  inverse_group_t invert_group(group_t const&);

  template<typename Key>
  void sort_crack_result(std::vector<crack_result<Key>>& vec) {
    std::sort(vec.rbegin(), vec.rend(), [](crack_result<Key>& a, crack_result<Key>& b) { return a.p_value < b.p_value; });
  }

  namespace caesar {
    void encrypt(string_t& str, key_t const& key, group_t const& group);
    void decrypt(string_t& str, key_t const& key, group_t const& group);

    // If the p_value is set to one, we will keep going to try to maximise the p-value
    std::vector<crack_result<key_t>> crack(prob_table observed, prob_table const& expected,
                                           group_t const& group, freq_t ptext_length,
                                           prob_t p_value = default_p_value);
    prob_t detect(prob_table const& observed, prob_table const& expected, freq_t count);
  }

  namespace vigenere {
    void encrypt(string_t& str, key_t const& key, group_t const& group);
    void decrypt(string_t& str, key_t const& key, group_t const& group);

    std::vector<crack_result<key_t>> crack(windowed_prob_table observed, prob_table const& expected,
                                           group_t const& group, freq_t ptext_length,
                                           prob_t p_value = default_p_value);
    prob_t detect(windowed_prob_table const& observed, prob_table const& expected, freq_t count);
  }

  namespace xor_single {
    void crypt(data& str, key_t const& key);
    inline void encrypt(data& str, key_t const& key) { crypt(str, key); }
    inline void decrypt(data& str, key_t const& key) { crypt(str, key); }

    std::vector<crack_result<key_t>> crack(prob_table observed, prob_table const& expected,
                                           freq_t ptext_length, prob_t p_value = default_p_value);
  }

  namespace xorcrypt {
    void crypt(data& str, key_t const& key);
    inline void encrypt(data& str, key_t const& key) { crypt(str, key); }
    inline void decrypt(data& str, key_t const& key) { crypt(str, key); }

    std::vector<crack_result<key_t>> crack(windowed_prob_table observed, prob_table const& expected,
                                           freq_t ptext_length, prob_t p_value = default_p_value);
  }
}
