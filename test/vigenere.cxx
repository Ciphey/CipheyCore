#include "gtest/gtest.h"

#include <ciphey/swig.hpp>
#include "common.hpp"

struct vigenere_test_elem {
  ciphey::string_t ctext, ptext;
  ciphey::vigenere::key_t key;
};

std::vector<vigenere_test_elem> vigenere_tests {
  {
    "kb pyu bac fwoct zhyf bayv ttw, qv t dtqwya vbejb eyvm bl pwocojxp, dmymtm mfg nbpub hd vpx ngzlmpa pgvp pfqu mfka agubhpa ptq dclgpmlq.",
    "it was the dover road that lay, on a friday night late in november, before the first of the persons with whom this history has business.",
    {'c', 'i', 't', 'y'}
  },
  {
    "odxo yh uvtpmhx. vapq bqddv mja",
    "call me ishmael. some years ago",
    {'m', 'd'}
  },
};

TEST(cipheyCore, vigenere) {
  for (auto& test : vigenere_tests) {
    for (size_t i = 1; i < 16; ++i) {
      if (i == test.key.size())
        continue;
      auto analysis = ciphey::analyse_string(test.ctext, i, ciphey::test::domain());
      auto res = ciphey::vigenere_crack(analysis, ciphey::test::expected(), ciphey::test::group());
      if (res.size() != 0)
        std::cerr << "Incorrect key size " << i << " has " << res.size() << " results" << std::endl;
    }

    auto analysis = ciphey::analyse_string(test.ctext, test.key.size(), ciphey::test::domain());

    auto res = ciphey::vigenere_crack(analysis, ciphey::test::expected(), ciphey::test::group());

    ciphey::vigenere::key_t actual_key = test.key;
    for (auto& i : actual_key)
      i -= 'a';

    std::cerr << res.size() << " options" << std::endl;

    for (size_t i = 0; i < res.size(); ++i) {
      if (res[i].key == actual_key) {
        std::cerr << "True key had p-value of " << res[i].p_value << " and was position " << i << std::endl;
        goto found;
      }
    }
    EXPECT_TRUE(false) << "Key was not found";
    found:

    auto tmp_ctext = test.ctext;
    ciphey::vigenere::decrypt(tmp_ctext, actual_key, ciphey::test::group());
    EXPECT_EQ(tmp_ctext, test.ptext);
    ciphey::vigenere::encrypt(tmp_ctext, actual_key, ciphey::test::group());
    EXPECT_EQ(tmp_ctext, test.ctext);
  }
}

TEST(cipheyCore, vigenereFail) {
  ciphey::string_t test_string = "damn, daniel! back at it again with the black and white vans!";

  auto analysis = ciphey::analyse_string(test_string, 9, ciphey::test::domain());

  auto res = ciphey::vigenere_crack(analysis, ciphey::test::expected(), ciphey::test::group());

  EXPECT_EQ(res.size(), 0);
}

