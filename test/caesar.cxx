#include "gtest/gtest.h"

#include <ciphey/swig.hpp>
#include "common.hpp"

using namespace std::string_literals;

struct test_elem {
  ciphey::string_t ctext, ptext;
  ciphey::caesar::key_t key;
};

static std::vector<test_elem> caesar_tests {
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
    "uryyb zl anzr vf orr naq v yvxr qbt naq png",
    "hello my name is bee and i like dog and cat",
    13
  },
  {
    "kvu'a il kbwlk jba kvdu aol dvvkz dov hyl lykvz (aopz zovbsk il mpul)",
    "don't be duped cut down the woods who are erdos (this should be fine)",
    7
  }
//  {
//    "te hld esp mpde zq etxpd, te hld esp hzcde zq etxpd",
//    "it was the best of times, it was the worst of times",
//    11
//  }
};

TEST(cipheyCore, caesarDetect) {
  for (auto& test : caesar_tests) {
    auto analysis = ciphey::analyse_string(test.ctext);
    auto score = ciphey::caesar_detect(analysis, ciphey::test::expected());
    std::cout << "Scored " << score << std::endl;
    EXPECT_GT(score, ciphey::default_p_value);
  }
}

TEST(cipheyCore, caesar) {
  for (auto& test : caesar_tests) {
    ciphey::freq_table freqs;
    ciphey::freq_analysis(freqs, test.ctext);
    ciphey::prob_table probs = ciphey::freq_conv(freqs);

    auto analysis = ciphey::analyse_string(test.ctext, ciphey::test::domain());

    auto res_collection = ciphey::caesar_crack(analysis, ciphey::test::expected(), ciphey::test::group());
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

    {
      ciphey::freq_table freqs;
      ciphey::freq_analysis(freqs, test.ptext);
      ciphey::prob_table probs = ciphey::freq_conv(freqs);

      float_t chi_sq_sum = 0.;

      for (auto i : probs) {
        auto exp = ciphey::test::expected().at(i.first);
        auto chi_sq = i.second - exp;
        chi_sq *= chi_sq / exp;
        chi_sq *= test.ptext.size();
        chi_sq_sum += chi_sq;
        if (chi_sq > 0.5)
          std::cout << (::isalnum(i.first) ? i.first : (int)i.first) << ": " << chi_sq << std::endl;
      }

      std::cout << "SUM "<< chi_sq_sum << ": " << freqs.size() << std::endl;
      {};
    }


//    {
//      ciphey::freq_analysis(freqs, test.ptext);
//      ciphey::prob_table tab = ciphey::test::expected();
//      for (auto i : freqs) {
//        if (tab[i.first] == 0)
//          std::cout << "Unexpected char " << (::isalnum(i.first) ? i.first : (int)i.first)
//                    << " has freq " << i.first << std::endl;
//      }

//      auto chisq = ciphey::run_chisq(ciphey::create_assoc_table(probs, ciphey::test::expected()));

//      std::cout << "Chi^2 stat: " << chisq << " with probability "
//                << 1-ciphey::chisq_cdf(test.ptext.size(), chisq)
//                << std::endl;
//    }
  }
}

TEST(cipheyCore, caesarFail) {
  std::string s = "\x10\x10\x00\x00\x01\x10\x11\x01\x10\x10\x00\x01\x00\x10\x01\x01\x10\x10\x01\x01\x10\x11\x10\x01\x10\x00\x00\x01\x11\x00\x01\x01\x11\x00\x10\x01\x00\x10\x10\x01\x01\x01\x11\x00\x11\x01\x00\x01\x01\x10\x00\x01\x01\x10\x11\x01\x00\x11\x10\x01\x11\x00\x11\x00\x01\x10\x01\x01\x10\x10\x10\x01\x01\x01\x11\x00\x11\x01\x00\x01\x01\x10\x00\x01\x00\x10\x11\x01\x10\x01\x00\x01\x01\x11\x10\x01\x10\x00\x10\x01\x11\x01\x10\x01\x01\x00\x01\x01\x10\x11\x10\x01\x01\x10\x01\x01\x00\x10\x11\x01\x10\x00\x00\x01\x01\x10\x10\x01\x11\x00\x10\x01\x00\x01\x11\x00\x10\x10\x01\x01\x00\x10\x01\x01\x01\x01\x10\x01\x11\x01\x10\x01\x01\x10\x01\x01\x10\x01\x00\x01\x01\x10\x00\x01\x00\x10\x11\x01\x10\x01\x00\x01\x01\x11\x10\x01\x10\x00\x10\x01\x11\x01\x10\x01\x01\x10\x00\x01\x10\x11\x10\x01\x11\x00\x10\x01\x10\x01\x11\x01\x10\x00\x00\x01\x01\x10\x10\x01\x10\x10\x01\x01\x10\x00\x11\x00\x10\x00\x00\x01\x01\x10\x10\x01\x11\x00\x10\x01\x10\x01\x11\x00\x01\x01\x11\x00\x11\x01\x01\x01\x01\x10\x01\x01\x11\x01\x10\x01\x11\x01\x11\x01\x01\x11\x10\x01\x01\x01\x10\x01\x11\x01\x10\x01\x01\x10\x00\x01\x11\x01\x01\x01\x01\x01\x10\x01\x01\x01\x11\x00\x10\x10\x00\x01\x00\x10\x01\x01\x10\x01\x01\x01"s;
  auto analysis = ciphey::analyse_string(s);
  auto res = ciphey::caesar_crack(analysis, ciphey::test::expected(), ciphey::test::group());
}

TEST(cipheyCore, caesarDetectFail) {
  std::string s = "\x10\x10\x00\x00\x01\x10\x11\x01\x10\x10\x00\x01\x00\x10\x01\x01\x10\x10\x01\x01\x10\x11\x10\x01\x10\x00\x00\x01\x11\x00\x01\x01\x11\x00\x10\x01\x00\x10\x10\x01\x01\x01\x11\x00\x11\x01\x00\x01\x01\x10\x00\x01\x01\x10\x11\x01\x00\x11\x10\x01\x11\x00\x11\x00\x01\x10\x01\x01\x10\x10\x10\x01\x01\x01\x11\x00\x11\x01\x00\x01\x01\x10\x00\x01\x00\x10\x11\x01\x10\x01\x00\x01\x01\x11\x10\x01\x10\x00\x10\x01\x11\x01\x10\x01\x01\x00\x01\x01\x10\x11\x10\x01\x01\x10\x01\x01\x00\x10\x11\x01\x10\x00\x00\x01\x01\x10\x10\x01\x11\x00\x10\x01\x00\x01\x11\x00\x10\x10\x01\x01\x00\x10\x01\x01\x01\x01\x10\x01\x11\x01\x10\x01\x01\x10\x01\x01\x10\x01\x00\x01\x01\x10\x00\x01\x00\x10\x11\x01\x10\x01\x00\x01\x01\x11\x10\x01\x10\x00\x10\x01\x11\x01\x10\x01\x01\x10\x00\x01\x10\x11\x10\x01\x11\x00\x10\x01\x10\x01\x11\x01\x10\x00\x00\x01\x01\x10\x10\x01\x10\x10\x01\x01\x10\x00\x11\x00\x10\x00\x00\x01\x01\x10\x10\x01\x11\x00\x10\x01\x10\x01\x11\x00\x01\x01\x11\x00\x11\x01\x01\x01\x01\x10\x01\x01\x11\x01\x10\x01\x11\x01\x11\x01\x01\x11\x10\x01\x01\x01\x10\x01\x11\x01\x10\x01\x01\x10\x00\x01\x11\x01\x01\x01\x01\x01\x10\x01\x01\x01\x11\x00\x10\x10\x00\x01\x00\x10\x01\x01\x10\x01\x01\x01"s;
  auto analysis = ciphey::analyse_string(s);
  EXPECT_FALSE(ciphey::caesar_detect(analysis, ciphey::test::expected()));
}
