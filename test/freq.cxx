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

TEST(pad, pad) {
  uint8_t key = 2;

  uint8_t i = 0;

  std::array<size_t, 256> arr = {0};

  do { ++arr[i], ++arr[i^key]; }
  while (i += 2*key, ++i);

  for (size_t i = 0; i < 256; ++i) {
    if (arr[i] != 1) {
      std::cout << i << ": " << arr[i] << std::endl;
    }
  }
}
