#include "ciphey/ciphers.hpp"

#include <atomic>
#include <thread>
#include <future>

namespace ciphey::vigenere {
  // We use a shared_future so that a copy is unnecessary
  using intermediate_res_t = std::vector<std::shared_future<std::vector<crack_result<caesar::key_t>>>>;

  void reduce(std::vector<crack_result<key_t>>& v, intermediate_res_t& imdt, prob_t p_value,
              std::vector<crack_result<caesar::key_t> const*>& prev_pos, size_t pos = 0) {
    auto& target = imdt[pos].get();
    // At the bottom layer of recursion, we fill in all the values
    if (pos == prev_pos.size()) {
       crack_result<key_t> base_candidate{.key = key_t(imdt.size()), .p_value = 1};
      for (size_t i = 0; i < pos; ++i) {
        auto& part = *prev_pos[i];
        base_candidate.key[i] = part.key;
        base_candidate.p_value *= part.p_value;
      }
      for (auto& final_key_part : target) {
        auto candidate_p_value = base_candidate.p_value * final_key_part.p_value;
        if (candidate_p_value < p_value)
          continue;
        v.push_back(base_candidate);
        v.back().key.back() = final_key_part.key;
        v.back().p_value *= final_key_part.p_value;
      }
    }
    else {
      for (auto& i : target) {
        prev_pos[pos] = &i;
        reduce(v, imdt, p_value, prev_pos, pos + 1);
      }
    }
  }

  std::vector<crack_result<key_t>> crack(windowed_prob_table observed, prob_table const& expected,
                                         group_t const& group, freq_t count, prob_t p_value) {
    std::vector<crack_result<key_t>> ret;
    intermediate_res_t imdt(observed.size());
    // Solve as distinct substitution cyphers, in parallel
    for (size_t i = 0; i < observed.size(); ++i)
      imdt[i] = std::async(std::launch::deferred, [&, i]() -> std::vector<crack_result<caesar::key_t>> {
        // We keep the p value so that we end up with too many, rather than too few
        return caesar::crack(std::move(observed[i]), expected, group, count / observed.size(), p_value);
      });

    // Now we reduce the lists, and kick out any which fail our p value
    std::vector<crack_result<caesar::key_t> const*> indexes(observed.size() - 1);
    reduce(ret, imdt, p_value, indexes);
    // Now we can sort by p-value
    sort_crack_result(ret);
    return ret;
  }

  void encrypt(string_t& str, key_t const& key, group_t const& group) {
    const auto inverse = invert_group(group);

    size_t i = 0;
    for (auto& c : str) {
      // Ignore letters we cannot find
      if (auto iter = inverse.find(c); iter != inverse.end()) {
        c = group[(iter->second + key[i % key.size()])%group.size()];
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
}
