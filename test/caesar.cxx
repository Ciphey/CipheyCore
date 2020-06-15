#include "gtest/gtest.h"

#include <ciphey/swig.hpp>
#include "common.hpp"

struct test_elem {
  ciphey::string_t ctext, ptext;
  ciphey::caesar::key_t key;
};

std::vector<test_elem> tests {
  {
    "xubbe, fbqduj",
    "hello, planet",
    16
  },
  {
    "te hld esp ozgpc czlo esle wlj, zy l qctolj ytrse wlep ty yzgpxmpc, mpqzcp esp qtcde zq esp apcdzyd htes hszx estd stdezcj sld mfdtypdd.",
    "it was the dover road that lay, on a friday night late in november, before the first of the persons with whom this history has business.",
    11
  },
  {
    "te hld esp mpde zq etxpd, te hld esp hzcde zq etxpd",
    "it was the best of times, it was the worst of times",
    11
  }
};

TEST(cipheyCore, caesar) {
  for (auto& test : tests) {
    ciphey::freq_table freqs;
    ciphey::freq_analysis(freqs, test.ctext);
    ciphey::prob_table probs = ciphey::freq_conv(freqs);

    auto analysis = ciphey::analyse_string(test.ctext);

    auto res_collection = ciphey::caesar_crack(analysis, ciphey::test::expected(), ciphey::test::group(), true);
    std::cout << res_collection.size() << " options" << std::endl;

    for (size_t i = 0; i < res_collection.size(); ++i) {
      if (res_collection[i].key == test.key) {
        std::cout << "True key had p-value of " << res_collection[i].p_value << " and was position " << i << std::endl;
        goto found;
      }
      else {
        std::cout << "False key " << res_collection[i].key << " had p-value of " << res_collection[i].p_value << std::endl;
      }
    }
    EXPECT_TRUE(false) << "Key was not found";
    found:
    auto tmp_ctext = test.ctext;
    ciphey::caesar::decrypt(test.ctext, test.key, ciphey::test::group());
    EXPECT_EQ(test.ctext, test.ptext);
    ciphey::caesar::encrypt(test.ctext, test.key, ciphey::test::group());
    EXPECT_EQ(test.ctext, tmp_ctext);
  }
}
