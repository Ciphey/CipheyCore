#pragma once

#include <ciphey/typedefs.hpp>

#include <string>
#include <map>
#include <set>
#include <vector>

#include <cmath>
#include <cstdint>

namespace ciphey {
  struct assoc_table_elem { prob_t observed; prob_t expected; };
  using assoc_table = std::vector<assoc_table_elem>;

  /// A simple chi squared contribution calculator
  ///
  /// @param count If set to 1 (which is default), no normalisation will occur.
  ///              Otherwise normalises the table to this count (by multiplying by count)
  float_t run_chisq(assoc_table const& assoc, freq_t count = 1);
  float_t closeness_chisq(prob_table const& observed, prob_table const& expected, freq_t count);
  float_t closeness_chisq(windowed_prob_table const& observed, prob_table const& expected, freq_t count);
  prob_t chisq_cdf(freq_t dof, float_t up_to);
  /// Goodness-of-fit test
  ///
  /// @returns the probability that, given the real distribution is the expected one, something at least this uncorrelated arises
  prob_t gof_test(assoc_table const& assoc, freq_t count);

  string_t generate_fuzz(prob_table const& tab, size_t len);

  // Shannon entropy calculation
  float_t information_content(data const& b);

  assoc_table create_assoc_table(prob_table const& observed, prob_table const& expected);
  assoc_table closeness_assoc(prob_table const& observed, prob_table const& expected);

  void freq_analysis(freq_table&, string_t const& str);
  size_t freq_analysis(windowed_freq_table&, string_t const& str, domain_t const& domain, size_t offset = 0);
  void freq_analysis(windowed_freq_table& tabs, string_t const& str, size_t offset = 0);

  prob_table freq_conv(freq_table const& freqs, freq_t total_len);
  prob_table freq_conv(freq_table const& freqs);
  windowed_prob_table freq_conv(windowed_freq_table& freqs, freq_t total_len);
  windowed_prob_table freq_conv(windowed_freq_table& freqs);

  size_t filter_missing(freq_table& target, prob_table const& lookup);
  size_t filter_missing(freq_table& target, domain_t const& domain);

  void filter_missing(prob_table& target, prob_table const& lookup);
}
