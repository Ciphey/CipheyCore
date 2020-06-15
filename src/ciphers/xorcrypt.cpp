#include "ciphey/ciphers.hpp"

#include "common.hpp"

#include <atomic>
#include <thread>
#include <future>

namespace ciphey::vigenere {
  std::vector<crack_result<key_t>> crack(windowed_prob_table observed, prob_table const& expected,
                                         freq_t count, prob_t p_value) {
    return detail::reducer<key_t, xor_single::key_t, xor_single::crack>::crack(observed, expected, count, p_value);
  }

  void crypt(data& str, key_t const& key) {
    for (size_t i = 0; i < str.size(); ++i)
      str[i] ^= key[i % key.size()];
  }
}
