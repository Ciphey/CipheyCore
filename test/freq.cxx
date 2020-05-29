#include "gtest/gtest.h"

#include <Ciphey/freq.hpp>

TEST(CipheyCore, chisq) {
  auto res = 1 - Ciphey::chisq_cdf(13, 22.36);

  ASSERT_LE(::abs(res - 0.05), 0.01);
}
