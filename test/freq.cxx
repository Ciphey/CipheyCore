#include "gtest/gtest.h"

#include <ciphey/freq.hpp>

#include "common.hpp"

TEST(cipheyCore, chisq) {
  auto res = 1 - ciphey::chisq_cdf(13, 22.36);

  ASSERT_LE(::abs(res - 0.05), 0.01);
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
