#pragma once

#include <ciphey/ausearch.hpp>
#include <ciphey/ciphers.hpp>

#include <memory>

namespace ciphey {
  // We use the same names for the various analysis functions to simplify the API

  // +-------------------------------------------------------------------------+
  // |                            SIMPLE ANALYSIS                              |
  // +-------------------------------------------------------------------------+
  struct simple_analysis_res {
    freq_table freqs;
    domain_t domain;
    freq_t len;
  };
  inline std::shared_ptr<simple_analysis_res> analyse_string(string_t str, domain_t domain = {}) {
    auto ret = std::make_shared<simple_analysis_res>();
    ret->domain = std::move(domain);
    if (ret->domain.size())
      ret->len = freq_analysis(ret->freqs, str, ret->domain);
    else {
      freq_analysis(ret->freqs, str);
      ret->len = str.size();
    }
    return ret;
  }
  inline std::shared_ptr<simple_analysis_res> start_analysis(domain_t domain = {}) {
    auto ret = std::make_shared<simple_analysis_res>();
    ret->domain = std::move(domain);
    ret->len = 0;
    return ret;
  }
  inline void continue_analysis(std::shared_ptr<simple_analysis_res> target, string_t str) {
    if (target->domain.size())
      target->len += freq_analysis(target->freqs, str, target->domain);
    else {
      freq_analysis(target->freqs, str);
      target->len += str.size();
    }
  }

  // +-------------------------------------------------------------------------+
  // |                           WINDOWED ANALYSIS                             |
  // +-------------------------------------------------------------------------+
  struct windowed_analysis_res {
    windowed_freq_table freqs;
    domain_t domain;
    freq_t len;
  };
  inline std::shared_ptr<windowed_analysis_res> analyse_string(string_t str, size_t window_size,
                                                               domain_t domain = {}) {
    auto ret = std::make_shared<windowed_analysis_res>();
    ret->domain = std::move(domain);
    ret->freqs.resize(window_size);
    // If we have no domain, then treat everything as in the domain
    if (ret->domain.size() == 0) {
      freq_analysis(ret->freqs, str);
      ret->len = str.size();
    }
    else
      ret->len = freq_analysis(ret->freqs, str, ret->domain);
//    ret->probs = freq_conv(ret->freqs, ret->len);
    return ret;
  }
  inline std::shared_ptr<windowed_analysis_res> start_analysis(size_t window_size, domain_t domain = {}) {
    auto ret = std::make_shared<windowed_analysis_res>();
    ret->domain = std::move(domain);
    ret->len = 0;
    return ret;
  }
  inline void continue_analysis(std::shared_ptr<windowed_analysis_res> target, string_t str) {
    if (target->domain.size()) {
      freq_analysis(target->freqs, str);
      target->len += str.size();
    }
    else
      target->len += freq_analysis(target->freqs, str, target->domain);

  }
//  inline void finish_analysis(std::shared_ptr<windowed_analysis_res> target) {
//    target->probs = freq_conv(target->freqs, target->len);
//  }

  // +-------------------------------------------------------------------------+
  // |                               STATISTICS                                |
  // +-------------------------------------------------------------------------+

  // TODO: add noise param
  inline string_t fuzz(std::shared_ptr<simple_analysis_res> in, size_t len) {
    return generate_fuzz(freq_conv(in->freqs, in->len), len);
  }

  inline prob_t chisq_test(std::shared_ptr<simple_analysis_res> in, prob_table expected) {
    return gof_test(create_assoc_table(freq_conv(in->freqs, in->len), expected), in->len);
  }

  inline float_t info_content(data in) {
    return information_content(in);
  }

  // +-------------------------------------------------------------------------+
  // |                              CRYPTOGRAPHY                               |
  // +-------------------------------------------------------------------------+
  inline std::vector<ciphey::crack_result<ciphey::caesar::key_t>> caesar_crack(std::shared_ptr<simple_analysis_res> in,
                                                                               prob_table expected, group_t group,
                                                                               prob_t p_value = default_p_value) {
    return caesar::crack(freq_conv(in->freqs, in->len), expected, group, in->len, p_value);
  }

  inline string_t caesar_decrypt(string_t str, ciphey::caesar::key_t key, group_t group) {
    caesar::decrypt(str, key, group);
    return str;
  }
  inline string_t caesar_encrypt(string_t str, ciphey::caesar::key_t key, group_t group) {
    caesar::encrypt(str, key, group);
    return str;
  }
  inline prob_t caesar_detect(std::shared_ptr<simple_analysis_res> in, prob_table expected) {
    return caesar::detect(freq_conv(in->freqs, in->len), expected, in->len);
  }

  inline std::vector<ciphey::crack_result<ciphey::vigenere::key_t>> vigenere_crack(std::shared_ptr<windowed_analysis_res> in,
                                                                                   prob_table expected,
                                                                                   group_t group,
                                                                                   prob_t p_value = default_p_value) {
    return vigenere::crack(freq_conv(in->freqs, in->len), expected, group, in->len, p_value);
  }
  inline string_t vigenere_decrypt(string_t str, ciphey::vigenere::key_t key, group_t group) {
    vigenere::decrypt(str, key, group);
    return str;
  }
  inline string_t vigenere_encrypt(string_t str, ciphey::vigenere::key_t key, group_t group) {
    vigenere::encrypt(str, key, group);
    return str;
  }
  inline prob_t vigenere_detect(std::shared_ptr<windowed_analysis_res> in, prob_table expected) {
    auto prob_tab = freq_conv(in->freqs, in->len);
    return vigenere::detect(prob_tab, expected, in->len);
  }
  struct vigenere_key_len_candidate {
    prob_t p_value;
    size_t len;
    std::shared_ptr<windowed_analysis_res> tab;
  };
  inline std::vector<vigenere_key_len_candidate> vigenere_likely_key_lens(string_t in, prob_table expected,
                                                                          domain_t const& domain,
                                                                          prob_t p_value = default_p_value) {
    auto res = vigenere::likely_key_lens(in, expected, domain, p_value);
    std::vector<vigenere_key_len_candidate> ret;
    ret.reserve(res.candidates.size());
    for (auto& i : res.candidates) {
      ret.push_back({
                      .p_value = i.p_value, .len = i.len,
                      .tab = std::make_shared<windowed_analysis_res>(windowed_analysis_res{
                        .freqs=std::move(i.tab),
                        .domain=domain,
                        .len=res.count_in_domain
                     })});
    }
    return ret;
  }


  // +-------------------------------------------------------------------------+
  // |                                AUSEARCH                                 |
  // +-------------------------------------------------------------------------+
  struct ausearch_res {
    float_t weight;
    size_t index;
  };

  inline ausearch_res ausearch_minimise(std::vector<ausearch_edge> input) {
    auto edges = ausearch::convert_edges(input);

    ausearch_res ret = {.weight = ausearch::minimise_edges(edges)};
    ret.index = (size_t)(edges.front() - input.data()) / sizeof(ausearch_edge const*);
    return ret;
  }

  inline data xor_single_crypt(data str, ciphey::xor_single::key_t key) {
    xor_single::crypt(str, key);
    return str;
  }
}
