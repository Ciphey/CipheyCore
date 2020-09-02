#pragma once

#include <ciphey/freq.hpp>
#include <ciphey/typedefs.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include <cstdint>

#include <ciphey/ciphers.swig.hxx>

namespace ciphey {
  constexpr prob_t default_p_value = 0.01;

  inverse_group_t invert_group(group_t const&);

  template<typename Key>
  void sort_crack_result(std::vector<crack_result<Key>>& vec) {
    std::sort(vec.rbegin(), vec.rend(), [](crack_result<Key>& a, crack_result<Key>& b) { return a.p_value < b.p_value; });
  }

  namespace caesar {
    void encrypt(string_ref_t str, key_t const& key, group_t const& group);
    void decrypt(string_ref_t str, key_t const& key, group_t const& group);

    // If the p_value is set to one, we will keep going to try to maximise the p-value
    std::vector<crack_result<key_t>> crack(prob_table observed, prob_table const& expected,
                                           group_t const& group, freq_t ptext_length,
                                           prob_t p_value = default_p_value);
    prob_t detect(prob_table const& observed, prob_table const& expected, freq_t count);
  }

  namespace vigenere {
    // For some reason, vigenere cracker is more open
    constexpr prob_t crack_p_value = 0.5;

    void encrypt(string_ref_t str, key_t const& key, group_t const& group);
    void decrypt(string_ref_t str, key_t const& key, group_t const& group);

    std::vector<crack_result<key_t>> crack(windowed_prob_table observed, prob_table const& expected,
                                           group_t const& group, freq_t ptext_length,
                                           prob_t p_value = crack_p_value);
    prob_t detect(windowed_prob_table const& observed, prob_table const& expected, freq_t count);
    key_len_res likely_key_lens(string_const_ref_t input, prob_table const& expected, domain_t const& domain,
                                prob_t p_value = default_p_value);
    inline key_len_res likely_key_lens(string_const_ref_t input, prob_table const& expected,
                                       prob_t p_value = default_p_value) {
      domain_t domain;
      for (auto& i : expected)
        domain.emplace(i.first);
      return likely_key_lens(input, expected, domain, p_value);
    }
  }

  namespace xor_single {
    void crypt(bytes_ref_t str, key_t key);
    inline void encrypt(bytes_ref_t str, key_t const& key) { crypt(str, key); }
    inline void decrypt(bytes_ref_t str, key_t const& key) { crypt(str, key); }

    std::vector<crack_result<key_t>> crack(prob_table observed, prob_table const& expected,
                                           freq_t ptext_length, prob_t p_value = default_p_value);
    prob_t detect(const prob_table& observed, prob_table const& expected, freq_t count);
  }

  namespace xorcrypt {
    void crypt(bytes_ref_t str, bytes_const_ref_t key);
    inline void encrypt(bytes_ref_t str, bytes_const_ref_t key) { crypt(str, key); }
    inline void decrypt(bytes_ref_t str, bytes_const_ref_t key) { crypt(str, key); }

    // A result of 1 means that it is not likely to be xorcrypt
    size_t guess_len(bytes_const_ref_t input);

    std::vector<crack_result<bytes_t>> crack(windowed_prob_table const& observed, prob_table const& expected,
                                             freq_t ptext_length, prob_t p_value = default_p_value);
    prob_t detect(windowed_prob_table const& observed, prob_table const& expected, freq_t count);
  }
}
