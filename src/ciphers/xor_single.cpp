#include <ciphey/ciphers.hpp>

#include <array>

namespace ciphey::xor_single {
  // Unsigned overflow *is* defined, so we can iterate until we wrap around to zero again
  //
  // That's how we cover all byte values

  void xor_prob_table(prob_table& observed, key_t key) {
    uint8_t i = 0;
    // We need to swap the XOR pairs, but we also need to avoid swapping back
    //
    // HACK: There must be a better way than this
    std::array<bool, 256> is_on = {false};
    for (size_t i = 0; i < 256; ++i) {
      if (!std::exchange(is_on[i], true)) {
        auto i_xor = i ^ key;
        is_on[i_xor] = true;
        std::swap(observed[i], observed[i_xor]);
      }
    }
  }

  void crypt(bytes_ref_t& str, key_t const& key) {
    for (auto& i : str)
      i ^= key;
  }

  std::vector<crack_result<key_t>> crack(prob_table observed, prob_table const& expected, freq_t count, prob_t p_value) {
    std::vector<crack_result<key_t>> ret;

    for (uint8_t key = 1; key != 0; ++key) {
      // Undo last one, and add our one (the -1'th term is 0, so no effect!)
      xor_prob_table(observed, key ^ (key - 1));
      if (auto key_p_value = gof_test(create_assoc_table(observed, expected), 256); key_p_value > p_value)
        ret.push_back({.key = key, .p_value = key_p_value });
    }

    sort_crack_result(ret);

    return ret;
  }
}
