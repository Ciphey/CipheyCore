#include <ciphey/ciphers.hpp>

#include <stdexcept>

#include <iostream>

namespace ciphey {
  inverse_group_t invert_group(group_t const& group) {
    inverse_group_t ret;
    for (size_t i = 0; i < group.size(); ++i)
      ret[group[i]] = i;
    return ret;
  }
}

namespace ciphey::caesar {
  void rotate_prob_table(prob_table& observed, group_t const& group) {
    auto iter = group.cbegin();
    while (true) {
      auto prev = iter++;
      if (iter == group.cend())
        break;
      std::swap(observed[*prev], observed[*iter]);
    }
  }

  std::vector<crack_result<key_t>> crack(prob_table observed, prob_table const& expected,
                                         group_t const& group, freq_t count, prob_t p_value) {
    if (group.size() == 0)
      throw std::invalid_argument{"Empty group given"};

    std::vector<crack_result<key_t>> ret;

    // We can use the p-value directly, as H_0 is that it *is* caesar

    // Check identity
    if (auto key_p_value = gof_test(create_assoc_table(observed, expected), count); key_p_value > p_value)
      ret.push_back({.key = 0, .p_value = key_p_value });

    rotate_prob_table(observed, group);
    for (key_t key = 1; key < group.size(); ++key, rotate_prob_table(observed, group))
      if (auto key_p_value = gof_test(create_assoc_table(observed, expected), count); key_p_value > p_value)
        ret.push_back({.key = key, .p_value = key_p_value });

    sort_crack_result(ret);

    return ret;
  }

  void decrypt(string_t& str, key_t const& key, group_t const& group) {
    // We can just encrypt with the inverse key
    encrypt(str, group.size() - key, group);
  }

  void encrypt(string_t& str, key_t const& key, group_t const& group) {
    auto inverse = invert_group(group);

    for (auto& i : str)
      // Ignore letters we cannot find
      if (auto iter = inverse.find(i); iter != inverse.end())
        i = group[(iter->second + key)%group.size()];
  }

  prob_t detect(prob_table const& observed, prob_table const& expected, freq_t count) {
    if (count == 0)
      return 0.;
    auto stat = closeness_chisq(observed, expected, count);
    return 1 - chisq_cdf(count - 1, stat);
  }
}
