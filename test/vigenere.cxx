#include "gtest/gtest.h"

#include <ciphey/swig.hpp>
#include "common.hpp"

TEST(cipheyCore, vigenere) {
  ciphey::string_t test_string = "kb pyu bac fwoct zhyf bayv ttw, qv t dtqwya vbejb eyvm bl pwocojxp, dmymtm mfg nbpub hd vpx ngzlmpa pgvp pfqu mfka agubhpa ptq dclgpmlq.";

  auto res = ciphey::vigenere_crack(test_string, ciphey::test::expected(), ciphey::test::group(), 4);

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

  ciphey::vigenere::decrypt(test_string, actual_key, ciphey::test::group());
  ASSERT_EQ(test_string,"it was the dover road that lay, on a friday night late in november, before the first of the persons with whom this history has business.");
}
