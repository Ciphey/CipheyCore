#include "gtest/gtest.h"

#include <Ciphey/candidates.hpp>

TEST(CipheyCore, caesar) {
  Ciphey::prob_table expected = {
    { 'e', 0.12702 },
    { 't', 0.09056 },
    { 'a', 0.08167 },
    { 'o', 0.07507 },
    { 'i', 0.06966 },
    { 'n', 0.06749 },
    { 's', 0.06327 },
    { 'h', 0.06094 },
    { 'r', 0.05987 },
    { 'd', 0.04253 },
    { 'l', 0.04025 },
    { 'c', 0.02782 },
    { 'u', 0.02758 },
    { 'm', 0.02406 },
    { 'w', 0.0236  },
    { 'f', 0.02228 },
    { 'g', 0.02015 },
    { 'y', 0.01974 },
    { 'p', 0.01929 },
    { 'b', 0.01492 },
    { 'v', 0.00978 },
    { 'k', 0.00772 },
    { 'j', 0.00153 },
    { 'x', 0.0015  },
    { 'q', 0.00095 },
    { 'z', 0.00074 },
  };

  std::basic_string<Ciphey::char_t> test_string = "te hld esp ozgpc czlo esle wlj, zy l qctolj ytrse wlep ty yzgpxmpc, mpqzcp esp qtcde zq esp apcdzyd htes hszx estd stdezcj sld mfdtypdd.";
  Ciphey::freq_table freqs;
  Ciphey::freq_analysis(freqs, test_string);
  Ciphey::prob_table probs;
  Ciphey::freq_conv(probs, freqs);

  Ciphey::filter_missing(probs, expected);

  Ciphey::group_t group = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
    'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z'
  };

  auto key = Ciphey::caesar::crack(probs, expected, group, test_string.size(), 0.50);

  ASSERT_EQ(key, 11);
}
