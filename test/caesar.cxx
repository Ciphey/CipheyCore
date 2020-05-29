#include "gtest/gtest.h"

#include <Ciphey/candidates.hpp>
#include "common.hpp"

TEST(CipheyCore, caesar) {
  Ciphey::string_t test_string = "te hld esp ozgpc czlo esle wlj, zy l qctolj ytrse wlep ty yzgpxmpc, mpqzcp esp qtcde zq esp apcdzyd htes hszx estd stdezcj sld mfdtypdd.";
  Ciphey::freq_table freqs;
  Ciphey::freq_analysis(freqs, test_string);
  Ciphey::prob_table probs;
  Ciphey::freq_conv(probs, freqs);

  Ciphey::filter_missing(probs, Ciphey::test::expected());

  auto res = Ciphey::caesar::crack(probs, Ciphey::test::expected(), Ciphey::test::group(), test_string.size());

  ASSERT_EQ(res.key, 11);
}
