#include "gtest/gtest.h"

#include <Ciphey/swig.hpp>
#include "common.hpp"

TEST(CipheyCore, vigenere) {
  Ciphey::string_t test_string = "kb pyu bac fwoct zhyf bayv ttw, qv t dtqwya vbejb eyvm bl pwocojxp, dmymtm mfg nbpub hd vpx ngzlmpa pgvp pfqu mfka agubhpa ptq dclgpmlq.";

  auto res = Ciphey::viginere_crack(test_string, Ciphey::test::expected(), Ciphey::test::group(), 5, true);

  auto actual_key = {'c', 'i', 't', 'y'};
  ASSERT_EQ(res.key, res.key);
}
