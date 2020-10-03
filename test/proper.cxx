#include "gtest/gtest.h"

#include <ciphey/proper.hpp>

#include "common.hpp"

TEST(cipheyCore, factor4) {
  auto res = ciphey::factorise("15");
  EXPECT_EQ(res.size(), 2);
  EXPECT_NE(std::find(res.begin(), res.end(), "3"), res.end());
  EXPECT_NE(std::find(res.begin(), res.end(), "5"), res.end());
}

TEST(cipheyCore, factor100) {
  std::string prime1 = "719486667183331";
  std::string prime2 = "800508251485823";

  auto res = ciphey::factorise(prime1 + "*" + prime2);
  EXPECT_EQ(res.size(), 2);
  EXPECT_NE(std::find(res.begin(), res.end(), prime1), res.end());
  EXPECT_NE(std::find(res.begin(), res.end(), prime2), res.end());
}

TEST(cipheyCore, factor200) {
  std::string prime1 = "946766187198366489795381762577";
  std::string prime2 = "308192359500787562218584749203";

  auto res = ciphey::factorise(prime1 + "*" + prime2);
  EXPECT_EQ(res.size(), 2);
  EXPECT_NE(std::find(res.begin(), res.end(), prime1), res.end());
  EXPECT_NE(std::find(res.begin(), res.end(), prime2), res.end());
}

//TEST(cipheyCore, factor300) {
//  std::string prime1 = "557371569950054887534203895142763047884619287";
//  std::string prime2 = "966876054254231105319885678326974155520190723";

//  auto res = ciphey::factorise(prime1 + "*" + prime2);
//  EXPECT_EQ(res.size(), 2);
//  EXPECT_NE(std::find(res.begin(), res.end(), prime1), res.end());
//  EXPECT_NE(std::find(res.begin(), res.end(), prime2), res.end());
//}
