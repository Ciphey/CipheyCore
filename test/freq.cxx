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

  EXPECT_EQ(ciphey::closeness_chisq(obs, exp, 10), 0);

  auto res = ciphey::closeness_chisq(obs, exp2, 10);
  EXPECT_LE(::abs(res - 0.58), 0.01);
}

TEST(cipheyCore, entropy) {
  auto* data_ptr = u8"\x0f\xf0\x03";
  auto len = 0;
  for (auto i = data_ptr; *i; ++i) ++len;

  ciphey::data data(data_ptr, data_ptr+len);

  std::cout << ciphey::information_content(data) << std::endl;

}
