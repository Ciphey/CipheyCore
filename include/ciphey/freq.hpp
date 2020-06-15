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

  float_t run_chisq(assoc_table const& assoc, freq_t count);
  float_t chisq_cdf(freq_t dof, float_t up_to);
  /// Goodness-of-fit test
  ///
  /// @returns the probability that, given the real distribution is the expected one, something at least this uncorrelated arises
  prob_t gof_chisq(assoc_table const& assoc, freq_t count);
  prob_t closeness_chisq(prob_table const& observed, prob_table const& expected, freq_t count);
  string_t generate_fuzz(prob_table const& tab, size_t len);

  assoc_table create_assoc_table(prob_table const& observed, prob_table const& expected);

  void freq_analysis(freq_table&, string_t const& str);
  size_t freq_analysis(windowed_freq_table&, string_t const& str, domain_t const& domain);
  void freq_analysis(windowed_freq_table& tabs, string_t const& str, size_t offset = 0);
  prob_table freq_conv(freq_table const& freqs, freq_t total_len);
  prob_table freq_conv(freq_table const& freqs);
  windowed_prob_table freq_conv(windowed_freq_table& freqs, freq_t total_len);
  windowed_prob_table freq_conv(windowed_freq_table& freqs);

  size_t filter_missing(freq_table& target, prob_table const& lookup);
  size_t filter_missing(freq_table& target, domain_t const& domain);

  void filter_missing(prob_table& target, prob_table const& lookup);
}
