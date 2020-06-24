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
    auto stat = closeness_chisq(observed, expected, count);
    return 1 - chisq_cdf(count - 1, stat);
  }
}
