#pragma once

#include <ciphey/ciphers.hpp>

#include <memory>

namespace ciphey {
  struct simple_analysis_res {
    freq_table freqs;
    prob_table probs;
    freq_t len = 0;
  };

  inline std::shared_ptr<simple_analysis_res> analyse_string(string_t str) {
    auto ret = std::make_shared<simple_analysis_res>();
    ret->len = str.size();
    freq_analysis(ret->freqs, str);
    freq_conv(ret->probs, ret->freqs);
    return ret;
  }

  inline std::shared_ptr<simple_analysis_res> start_analysis() {
    return std::make_shared<simple_analysis_res>();
  }

  inline void continue_analysis(std::shared_ptr<simple_analysis_res> target, string_t str) {
    target->len += str.size();
    freq_analysis(target->freqs, str);
  }

  inline void finish_analysis(std::shared_ptr<simple_analysis_res> target) {
    freq_conv(target->probs, target->freqs);
  }

  // TODO: add noise param
  inline string_t fuzz(std::shared_ptr<simple_analysis_res> const& tab, size_t len) {
    return generate_fuzz(tab->probs, len);
  }

  inline prob_t chisq_test(std::shared_ptr<simple_analysis_res> in, prob_table expected,
                           bool do_filter_missing = true) {
    assoc_table tab;
    if (do_filter_missing) {
      auto cpy = in->probs;
      filter_missing(cpy, expected);
      tab = create_assoc_table(cpy, expected);
    }
    else
      tab = create_assoc_table(in->probs, expected);
    return gof_chisq(tab, in->len);
  }

  inline std::vector<ciphey::crack_result<ciphey::caesar::key_t>> caesar_crack(std::shared_ptr<simple_analysis_res> in,
                                                                               prob_table expected, group_t group,
                                                                               bool do_filter_missing = true,
                                                                               prob_t p_value = default_p_value) {
    if (do_filter_missing) {
      prob_table tab = in->probs;
      filter_missing(tab, expected);
      return caesar::crack(std::move(tab), expected, group, in->len, p_value);
    }

    return caesar::crack(in->probs, expected, group, in->len, p_value);
  }

  inline string_t caesar_decrypt(string_t str, ciphey::caesar::key_t key, group_t group) {
    caesar::decrypt(str, key, group);
    return str;
  }
  inline string_t caesar_encrypt(string_t str, ciphey::caesar::key_t key, group_t group) {
    caesar::encrypt(str, key, group);
    return str;
  }

  inline std::vector<ciphey::crack_result<ciphey::vigenere::key_t>> vigenere_crack(string_t str,
                                                                                   prob_table const& expected,
                                                                                   group_t const& group, size_t key_length,
                                                                                   bool do_filter_missing = true,
                                                                                   prob_t p_value = default_p_value) {
    windowed_freq_table freqs(key_length);
    if (do_filter_missing) {
      std::set<char_t> domain;
      for (auto& i : expected)
        domain.emplace(i.first);
      freq_analysis(freqs, str, domain);
    }
    else
      freq_analysis(freqs, str);

    windowed_prob_table probs;
    probs.reserve(key_length);
    for (auto& i : freqs)
      freq_conv(probs.emplace_back(), i);

    return vigenere::crack(probs, expected, group, str.length(), p_value);
  }

  inline string_t vigenere_decrypt(string_t str, ciphey::vigenere::key_t key, group_t group) {
    vigenere::decrypt(str, key, group);
    return str;
  }

  inline string_t vigenere_encrypt(string_t str, ciphey::vigenere::key_t key, group_t group) {
    vigenere::encrypt(str, key, group);
    return str;
  }
}
