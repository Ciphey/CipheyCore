#include "gtest/gtest.h"

#include <ciphey/candidates.hpp>
#include "common.hpp"

TEST(cipheyCore, caesar) {
  ciphey::string_t test_string = "te hld esp ozgpc czlo esle wlj, zy l qctolj ytrse wlep ty yzgpxmpc, mpqzcp esp qtcde zq esp apcdzyd htes hszx estd stdezcj sld mfdtypdd.";
  ciphey::freq_table freqs;
  ciphey::freq_analysis(freqs, test_string);
  ciphey::prob_table probs;
  ciphey::freq_conv(probs, freqs);

  ciphey::filter_missing(probs, ciphey::test::expected());

  auto res_collection = ciphey::caesar::crack(probs, ciphey::test::expected(), ciphey::test::group(), test_string.size());
  std::cerr << res_collection.size() << " options" << std::endl;

  for (size_t i = 0; i < res_collection.size(); ++i) {
    if (res_collection[i].key == 11) {
      std::cerr << "True key had p-value of " << res_collection[i].p_value << " and was position " << i << std::endl;
      goto found;
    }
  }
  EXPECT_TRUE(false) << "Key was not found";
  found:
  auto& res = res_collection.front();
  ASSERT_EQ(res.key, 11);
  ciphey::caesar::decrypt(test_string, res.key, ciphey::test::group());
  ASSERT_EQ(test_string,"it was the dover road that lay, on a friday night late in november, before the first of the persons with whom this history has business.");
}
