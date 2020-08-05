#include "ciphey/ciphers.hpp"

#include "common.hpp"

#include <algorithm>
#include <atomic>
#include <thread>
#include <future>

namespace ciphey::xorcrypt {
  size_t guess_len(bytes_const_ref_t input) {
    std::pair<size_t, float_t> best_candidate = { 0, std::numeric_limits<float_t>::infinity() };

    size_t max_len = input.size() / 4;
    if (max_len < 1)
      throw std::invalid_argument("Too little ciphertext to perform accurate analysis!");

    for (size_t candidate = 1; candidate < max_len; ++candidate) {
      auto len = input.size();
      auto tail = input.size() % candidate;
      auto n_chunks = input.size() / candidate;
      size_t end = input.size() - tail - candidate;

      freq_t weight = 0;

      // We have to work on full blocks, or we end up with uneven weights
      //
      // We don't have to bounds check here, as we skip 1 (starting on 2), and we have at least 1 other block
      for (size_t chunk_begin = 0; chunk_begin < end; chunk_begin += candidate)
        weight += hamming_distance(input.subspan(chunk_begin, candidate),
                                   input.subspan(chunk_begin + candidate, candidate));

      float_t normalised_weight = weight;
      normalised_weight /= (n_chunks - 2); // For some reason, 2 is the number that works here
      normalised_weight /= candidate;      // I like 2, it's a great number!

      if (normalised_weight < best_candidate.second) {
        best_candidate.first = candidate;
        best_candidate.second = normalised_weight;
      }
    }

    return best_candidate.first;
  }

  std::vector<crack_result<bytes_t>> crack(const windowed_prob_table& observed, prob_table const& expected,
                                           freq_t count, prob_t p_value) {
    return detail::reducer<key_t, xor_single::key_t, xor_single::crack>::crack(observed, expected, count, p_value);
  }

  void crypt(bytes_ref_t str, bytes_const_ref_t key) {
    for (size_t i = 0; i < str.size(); ++i)
      str[i] ^= key[i % key.size()];
  }

  prob_t detect(windowed_prob_table const& observed, prob_table const& expected, freq_t count) {
    if (count == 0)
      return 0.;

    prob_t acc = 1.;
    for (auto& i : observed) {
      // FIXME: work out the amount from the count, rather than just flooring it
      acc *= xor_single::detect(i, expected, count / observed.size());
    }
    return acc;
  }
}
