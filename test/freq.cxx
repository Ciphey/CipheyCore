#include "gtest/gtest.h"

#include <Ciphey/freq.hpp>

TEST(CipheyCore, chisq) {
  auto res = Ciphey::critical_chisq(13, 0.05);

  ASSERT_LE(::abs(res - 22.36), 0.01);
}
