#pragma once

#include <Ciphey/typedefs.hpp>

#include <string>
#include <map>
#include <vector>

#include <cmath>
#include <cstdint>

namespace Ciphey {
  struct assoc_table_elem { char_t name; prob_t observed; prob_t expected; };
  using assoc_table = std::vector<assoc_table_elem>;

  float_t run_chisq(assoc_table const& assoc, freq_t count);
  float_t critical_chisq(freq_t dof, prob_t p_value);
  // Goodness-of-fit test
  bool gof_chisq(assoc_table const& assoc, freq_t count, prob_t p_value);

  assoc_table create_assoc_table(prob_table const& observed, prob_table const& expected);

  void freq_analysis(freq_table&, string_t const& str);
  void freq_conv(prob_table&, freq_table const& freqs);
  void freq_conv(prob_table&, freq_table const& freqs, freq_t total_len);

  void filter_missing(prob_table& target, prob_table const& lookup);
}
