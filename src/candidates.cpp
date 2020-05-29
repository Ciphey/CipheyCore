#include <Ciphey/candidates.hpp>

#include <Ciphey/freq.hpp>

#include <atomic>
#include <thread>
#include <stdexcept>

namespace Ciphey {
  std::map<size_t, char_t> invert_group(group_t const& group) {
    std::map<size_t, char_t> ret;
    for (size_t i = 0; i < group.size(); ++i)
      ret[i] = group[i];
    return ret;
  }
}

namespace Ciphey::caesar {
  void rotate_prob_table(prob_table& observed, group_t const& group) {
    auto iter = group.cbegin();
    while (true) {
      auto prev = iter++;
      if (iter == group.cend())
        break;
      std::swap(observed[*prev], observed[*iter]);
    }
  }

  crack_results<key_t> crack(prob_table observed, prob_table const& expected, group_t const& group, freq_t count, prob_t p_value) {
    if (group.size() == 0)
      throw std::invalid_argument{"Empty group given"};

    // We want to maximise the key's p value up to `p_value`
    crack_results<key_t> ret = { 0 };

    rotate_prob_table(observed, group);
    for (key_t key = 1; key < group.size(); ++key, rotate_prob_table(observed, group)) {
      auto key_p_value = gof_chisq(create_assoc_table(observed, expected), group.size() - 1);
      if (key_p_value > p_value)
        return {.key = key, .p_value = key_p_value };
      else if (key_p_value > ret.p_value)
        ret = {.key = key, .p_value = key_p_value };
    }
    // On failure, we return the lowest p_value
    return ret;
  }

  void crypt(string_t& str, key_t const& key, group_t const& group) {
    auto inverse = invert_group(group);

    for (auto& i : str)
      // Ignore letters we cannot find
      if (auto iter = inverse.find(i); iter != inverse.end())
        i = group[(i + key)%group.size()];
  }
}

namespace Ciphey::vigenere {
  crack_results<key_t> crack(windowed_prob_table observed, prob_table const& expected, group_t const& group, freq_t count, prob_t p_value) {
    crack_results<key_t> ret;
    ret.p_value = 1;
    ret.key.resize(observed.size());
    // Solve as distinct substitution cyphers
    for (size_t i = 0; i < observed.size(); ++i) {
      auto res = caesar::crack(std::move(observed[i]), expected, group, count / observed.size(),
                               // We are multiplying the probabilities, so this ensures it stays below p_value
                               ::powf(p_value, 1./observed.size()));
      ret.key[i] = res.key;
      // The probability is the product of all the child probabilities
      ret.p_value *= res.p_value;
    }

    return ret;
  }
}
