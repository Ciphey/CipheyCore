#include "gtest/gtest.h"

#include <Ciphey/freq.hpp>

TEST(CipheyCore, chisq) {
  auto res = 1 - Ciphey::chisq_cdf(13, 22.36);

  ASSERT_LE(::abs(res - 0.05), 0.01);
}


TEST(CipheyCore, windowed_freq) {
  Ciphey::string_t str = "abcadcabcabc1d3";
  Ciphey::windowed_freq_table tab(3);
  Ciphey::freq_analysis(tab, str);

  Ciphey::windowed_freq_table true_table = {
    {{'a', 4}, {'1', 1}},
    {{'b', 3}, {'d', 2}},
    {{'c', 4}, {'3', 1}},
  };

  ASSERT_EQ(tab, true_table);
}
