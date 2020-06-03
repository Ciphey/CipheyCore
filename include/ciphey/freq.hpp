#pragma once

#include <ciphey/typedefs.hpp>

#include <string>
#include <map>
#include <set>
#include <vector>

#include <cmath>
#include <cstdint>

namespace ciphey {
  struct assoc_table_elem { char_t name; prob_t observed; prob_t expected; };
  using assoc_table = std::vector<assoc_table_elem>;

  float_t run_chisq(assoc_table const& assoc, freq_t count);
  float_t chisq_cdf(freq_t dof, float_t up_to);
  /// Goodness-of-fit test
  ///
  /// @returns the probability that, given the real distribution is the expected one, something at least this uncorrelated arises
  prob_t gof_chisq(assoc_table const& assoc, freq_t count);
  string_t generate_fuzz(prob_table const& tab, size_t len);

  assoc_table create_assoc_table(prob_table const& observed, prob_table const& expected);

  void freq_analysis(freq_table&, string_t const& str);
  void freq_analysis(windowed_freq_table&, string_t const& str);
  void freq_analysis(windowed_freq_table&, string_t const& str, std::set<char_t> domain);
  void freq_conv(prob_table&, freq_table const& freqs);
  void freq_conv(prob_table&, freq_table const& freqs, freq_t total_len);

  void filter_missing(prob_table& target, prob_table const& lookup);
}
