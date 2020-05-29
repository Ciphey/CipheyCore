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

  key_t crack(prob_table observed, prob_table const& expected, group_t const& group, freq_t count, prob_t p_value) {
    if (group.size() == 0)
      throw std::invalid_argument{"Empty group given"};

    rotate_prob_table(observed, group);
    for (key_t key = 1; key < group.size(); ++key, rotate_prob_table(observed, group))
      if (gof_chisq(create_assoc_table(observed, expected), group.size() - 1, p_value))
        return key;
    return 0;
  }

  void crypt(string_t& str, key_t const& key, group_t const& group) {
    auto inverse = invert_group(group);

    for (auto& i : str)
      // Ignore letters we cannot find
      if (auto iter = inverse.find(i); iter != inverse.end())
        i = group[(i + key)%group.size()];
  }
}

//namespace Ciphey::vigenere {
//  void crypt(string_t& str, const std::vector<uint8_t>& key) {
//    for (size_t i = 0; i < str.size(); ++i) {

//    }
//  }

//  key_t crack_context::get_next() {
//    std::atomic<bool> stop = false;
//    // We can easily be parallel over the elements of the key
//  }
//}
