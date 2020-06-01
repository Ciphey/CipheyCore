#pragma once

#include <ciphey/freq.hpp>
#include <ciphey/typedefs.hpp>

#include <string>
#include <vector>

#include <cstdint>

#include <ciphey/candidates.swig.hxx>

namespace ciphey {
  constexpr prob_t default_p_value = 0.01f;

  inverse_group_t invert_group(group_t const&);

  namespace caesar {
    void decrypt(string_t& str, key_t const& key, group_t const& group);

    // If the p_value is set to one, we will keep going to try to maximise the p-value
    std::vector<crack_result<key_t>> crack(prob_table observed, prob_table const& expected,
                                           group_t const& group, freq_t count,
                                           prob_t p_value = default_p_value);
  }

  namespace vigenere {
    /// A sequence of offsets
    using key_t = std::vector<size_t>;
    void decrypt(string_t& str, key_t const& key, group_t const& group);

    std::vector<crack_result<key_t>> crack(windowed_prob_table observed, prob_table const& expected,
                                           group_t const& group, freq_t count,
                                           prob_t p_value = default_p_value);
  }
}
