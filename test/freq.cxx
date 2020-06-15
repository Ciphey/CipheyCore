#include "gtest/gtest.h"

#include <ciphey/freq.hpp>

#include "common.hpp"

TEST(cipheyCore, chisq) {
  auto res = 1 - ciphey::chisq_cdf(13, 22.36);

  ASSERT_LE(::abs(res - 0.05), 0.01);
}

TEST(cipheyCore, chisqGof) {
  std::string short_str = "hello, planet";

  ciphey::freq_table freq_tab;
  ciphey::freq_analysis(freq_tab, short_str);

  auto n_removed = ciphey::filter_missing(freq_tab, ciphey::test::domain());
  auto new_len = short_str.size() - n_removed;

  auto prob_tab = ciphey::freq_conv(freq_tab);
  auto assoc = ciphey::create_assoc_table(prob_tab, ciphey::test::expected());

  float_t obs_sum = 0, exp_sum = 0;
  for (auto& i : assoc) {
    obs_sum += i.observed; exp_sum += i.expected;
  }

  ASSERT_LE(::abs(obs_sum - 1), 0.001);
  ASSERT_LE(::abs(exp_sum - 1), 0.001);

  auto stat = ciphey::gof_chisq(assoc, new_len);
  EXPECT_GT(stat, ciphey::default_p_value);
}


TEST(cipheyCore, windowed_freq) {
  ciphey::string_t str = "abcadcabcabc1d3";
  ciphey::windowed_freq_table tab(3);
  ciphey::freq_analysis(tab, str);

  ciphey::windowed_freq_table true_table = {
    {{'a', 4}, {'1', 1}},
    {{'b', 3}, {'d', 2}},
    {{'c', 4}, {'3', 1}},
  };

  ASSERT_EQ(tab, true_table);
}
