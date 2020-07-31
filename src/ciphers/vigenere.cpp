#include "ciphey/ciphers.hpp"

#include "common.hpp"

#include <atomic>
#include <thread>
#include <future>

namespace ciphey::vigenere {
  std::vector<crack_result<key_t>> crack(windowed_prob_table observed, prob_table const& expected,
                                         group_t const& group, freq_t count, prob_t p_value) {
    return detail::reducer<key_t, caesar::key_t, caesar::crack, group_t const&>::crack(observed, expected, count, p_value, group);
  }

  void encrypt(string_t& str, key_t const& key, group_t const& group) {
    const auto inverse = invert_group(group);

    size_t i = 0;
    for (auto& c : str) {
      // Ignore letters we cannot find
      if (auto iter = inverse.find(c); iter != inverse.end()) {
        c = group[(iter->second + key[i % key.size()]) % group.size()];
        ++i;
      }
    }
  }

  void decrypt(string_t& str, key_t const& key, group_t const& group) {
    std::vector<size_t> inv_key(key.size());
    for (size_t i = 0; i < key.size(); ++i)
      inv_key[i] = group.size() - key[i];

    encrypt(str, inv_key, group);
  }

  prob_t detect(windowed_prob_table const& observed, prob_table const& expected, freq_t count) {
    if (count == 0)
      return 0.;

    prob_t acc = 1.;
    for (auto& i : observed) {
      // FIXME: work out the amount from the count, rather than just flooring it
      acc *= caesar::detect(i, expected, count / observed.size());
    }
    return acc;

//    auto stat = closeness_chisq(observed, expected, count);
//    return 1-chisq_cdf(count - observed.size(), stat);
  }

  key_len_res likely_key_lens(std::string input, prob_table const& expected,
                              domain_t const& domain, prob_t p_value) {
    key_len_res ret;
    ret.candidates.reserve(8);

    // Dividing by 4 is a good guess of what is feasible to crack
    for (size_t key_len = 2; key_len < input.size() / 8; ++key_len) {

      ret.candidates.emplace_back();
      auto& last = ret.candidates.back();
      last.tab = windowed_freq_table(key_len);
      // I don't think the extra write here has a significant effect of performance
      ret.count_in_domain = freq_analysis(last.tab, input, domain);
      auto observed = freq_conv(last.tab, ret.count_in_domain);

      if (auto prob = detect(observed, expected, ret.count_in_domain); prob > p_value) {
        last.len = key_len;
        last.p_value = prob;
      }
      else
        ret.candidates.pop_back();
    }

    std::sort(ret.candidates.rbegin(), ret.candidates.rend(), [](auto& a, auto& b) { return a.p_value < b.p_value; });

    return ret;
  }
}
