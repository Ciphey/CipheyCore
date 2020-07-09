#include "gtest/gtest.h"

#include <ciphey/swig.hpp>
#include "common.hpp"

TEST(cipheyCore, vigenere) {
  ciphey::string_t test_string = "kb pyu bac fwoct zhyf bayv ttw, qv t dtqwya vbejb eyvm bl pwocojxp, dmymtm mfg nbpub hd vpx ngzlmpa pgvp pfqu mfka agubhpa ptq dclgpmlq.";
  const ciphey::string_t expected_string = "it was the dover road that lay, on a friday night late in november, before the first of the persons with whom this history has business.";

  auto analysis = ciphey::analyse_string(test_string, 4, ciphey::test::domain());

  auto res = ciphey::vigenere_crack(analysis, ciphey::test::expected(), ciphey::test::group());

  ciphey::vigenere::key_t actual_key = {'c', 'i', 't', 'y'};
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

  auto tmp_ctext = test_string;
  ciphey::vigenere::decrypt(test_string, actual_key, ciphey::test::group());
  EXPECT_EQ(test_string, expected_string);
  ciphey::vigenere::encrypt(test_string, actual_key, ciphey::test::group());
  EXPECT_EQ(test_string, tmp_ctext);
}

TEST(cipheyCore, vigenereFail) {
  ciphey::string_t test_string = "damn, daniel! back at it again with the black and white vans!";

  auto analysis = ciphey::analyse_string(test_string, 9, ciphey::test::domain());

  auto res = ciphey::vigenere_crack(analysis, ciphey::test::expected(), ciphey::test::group());

  EXPECT_EQ(res.size(), 0);
}

