#include "gtest/gtest.h"

#include <ciphey/freq.hpp>

#include "common.hpp"

TEST(cipheyCore, chisq) {
  auto res = 1 - ciphey::chisq_cdf(13, 22.36);

  ASSERT_LE(::abs(res - 0.05), 0.01);
}

TEST(cipheyCore, chisqGof) {
  std::string short_str = "hello, planet";

  ciphey::prob_table eng_dist = {
    { 'e', 11.162e-2, },
    { 't', 9.356e-2 },
    { 'a', 8.497e-2 },
    { 'r', 7.587e-2, },
    { 'i', 7.507e-2, },
    { 'o', 6.749e-2, },
    { 'n', 6.327e-2, },
    { 's', 6.094e-2, },
    { 'h', 6.094e-2, },
    { 'd', 4.253e-2, },
    { 'l', 4.025e-2, },
    { 'u', 2.758e-2, },
    { 'w', 2.560e-2, },
    { 'm', 2.406e-2, },
    { 'f', 2.228e-2, },
    { 'c', 2.202e-2, },
    { 'g', 2.015e-2, },
    { 'y', 1.994e-2, },
    { 'p', 1.929e-2, },
    { 'b', 1.492e-2, },
    { 'k', 1.292e-2, },
    { 'v', 0.978e-2, },
    { 'j', 0.153e-2, },
    { 'x', 0.150e-2, },
    { 'q', 0.095e-2, },
    { 'z', 0.077e-2 },
  };

  ciphey::freq_table freq_tab;
  ciphey::freq_analysis(freq_tab, short_str);

  auto n_removed = ciphey::filter_missing(freq_tab, ciphey::test::domain());
  auto new_len = short_str.size() - n_removed;

  auto prob_tab = ciphey::freq_conv(freq_tab);
  auto assoc = ciphey::create_assoc_table(prob_tab, eng_dist);

  float_t obs_sum = 0, exp_sum = 0;
  for (auto& i : assoc) {
    obs_sum += i.observed; exp_sum += i.expected;
  }

  EXPECT_LE(::abs(obs_sum - 1), 0.001);
  EXPECT_LE(::abs(exp_sum - 1), 0.001);

  auto stat = ciphey::gof_test(assoc, new_len);
  EXPECT_GT(stat, ciphey::default_p_value);
}


TEST(cipheyCore, windowedFreq) {
  ciphey::string_t str = "abcadcabcabczdy";
  ciphey::windowed_freq_table tab(3);
  ciphey::freq_analysis(tab, str);

  ciphey::windowed_freq_table true_table = {
    {{'a', 4}, {'z', 1}},
    {{'b', 3}, {'d', 2}},
    {{'c', 4}, {'y', 1}},
  };

  ASSERT_EQ(tab, true_table);
}

TEST(cipheyCore, windowedFreqDomain) {
  ciphey::string_t str = "abc adcabca   bczdy";
  ciphey::windowed_freq_table tab(3);
  auto len = ciphey::freq_analysis(tab, str, ciphey::test::domain());
  EXPECT_EQ(len, str.size() - 4);

  ciphey::windowed_freq_table true_table = {
    {{'a', 4}, {'z', 1}},
    {{'b', 3}, {'d', 2}},
    {{'c', 4}, {'y', 1}},
  };

  ASSERT_EQ(tab, true_table);
}


TEST(cipheyCore, simpleCloseness) {
  ciphey::prob_table obs  = {{'a', 0.5}, {'b', 0.2}, {'c', 0.3}};
  ciphey::prob_table exp  = {{'c', 0.5}, {'b', 0.2}, {'a', 0.3}};
  ciphey::prob_table exp2 = {{'c', 0.4}, {'b', 0.3}, {'a', 0.3}};

  EXPECT_EQ(ciphey::closeness_test(obs, exp, 10), 1);

  EXPECT_GT(ciphey::closeness_test(obs, exp2, 10), 0);
}

TEST(cipheyCore, entropy) {
  auto* data_ptr = u8"\x0f\xf0\x03";
  auto len = 0;
  for (auto i = data_ptr; *i; ++i) ++len;

  ciphey::bytes_t data(data_ptr, data_ptr+len);

  std::cout << ciphey::information_content(data) << std::endl;
}

TEST(cipheyCore, hammingDistance) {
  uint8_t const* x = reinterpret_cast<uint8_t const*>(u8"this is a test");
  uint8_t const* y = reinterpret_cast<uint8_t const*>(u8"wokka wokka!!!");

  size_t len = 14;

  ASSERT_EQ(ciphey::hamming_distance({x, len}, {y, len}), 37);
}
