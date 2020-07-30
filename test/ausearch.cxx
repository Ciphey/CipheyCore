#include "gtest/gtest.h"

#include <ciphey/ausearch.hpp>
#include "common.hpp"

TEST(cipheyCore, ausearch) {
  std::vector<ciphey::ausearch_edge> edges = {
    {0.9, 0.4, 0.2},
    {0.5, 0.1, 0.3},
    {0.6, 0.4, 0.4},
    {0.99, 10, 15},
    {0.1, 0.01, 0.001},
    {0.8, 0.2, 0.4},
    {0.3, 0.6, 0.3},
    {0.91, 4.2, 0.3},
  };

  auto conv = ciphey::ausearch::convert_edges(edges);
  ciphey::ausearch::minimise_edges(conv);
  auto x = ciphey::ausearch::calculate_weight(conv);

  EXPECT_LT(x, 0.306);

  for (uint_fast32_t i = 0; i < 1e4; ++i) {
    std::random_shuffle(edges.begin(), edges.end());
    conv = ciphey::ausearch::convert_edges(edges);
    ciphey::ausearch::minimise_edges(conv);
    EXPECT_EQ(x, ciphey::ausearch::calculate_weight(conv));
  }
}
