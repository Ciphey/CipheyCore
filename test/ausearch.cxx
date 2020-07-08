#include "gtest/gtest.h"

#include <ciphey/ausearch.hpp>
#include "common.hpp"

TEST(cipheyCore, ausearch) {
  std::vector<ciphey::ausearch_node> nodes = {
    {.success_probability = 0.9, .success_time=0.4, . failure_time=0.2},
    {.success_probability = 0.5, .success_time=0.1, . failure_time=0.3},
    {.success_probability = 0.6, .success_time=0.4, . failure_time=0.4},
    {.success_probability = 0.99, .success_time=10, . failure_time=15},
    {.success_probability = 0.1, .success_time=0.01, . failure_time=0.001},
    {.success_probability = 0.8, .success_time=0.2, . failure_time=0.4},
    {.success_probability = 0.3, .success_time=0.6, . failure_time=0.3},
    {.success_probability = 0.91, .success_time=4.2, . failure_time=0.3},
  };

  auto conv = ciphey::ausearch::convert_nodes(nodes);
  ciphey::ausearch::minimise_nodes(conv);
  std::cout << ciphey::ausearch::calculate_weight(conv) << std::endl;
  abort();
}
