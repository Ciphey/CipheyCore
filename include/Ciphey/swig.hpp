#pragma once

#include <Ciphey/candidates.hpp>

#include <memory>

namespace Ciphey {
  struct simple_analysis_res {
    freq_table freqs;
    prob_table probs;
    string_t val;
  };

  inline std::shared_ptr<const simple_analysis_res> analyse_string(string_t str) {
    auto ret = std::make_shared<simple_analysis_res>();
    ret->val = std::move(str);
    freq_analysis(ret->freqs, ret->val);
    freq_conv(ret->probs, ret->freqs);
    return ret;
  }

  inline Ciphey::caesar::key_t caesar_crack(std::shared_ptr<const simple_analysis_res> in, prob_table expected,
                                            group_t group, prob_t p_value, bool do_filter_missing = false) {
    if (do_filter_missing) {
      prob_table tab = in->probs;
      filter_missing(tab, expected);
      return caesar::crack(std::move(tab), expected, group, in->val.length(), p_value);
    }

    return caesar::crack(in->probs, expected, group, in->val.length(), p_value);
  }
}
