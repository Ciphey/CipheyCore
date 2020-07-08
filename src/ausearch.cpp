#include <ciphey/ausearch.hpp>

#include <list>
#include <numeric>
#include <utility>

namespace ciphey::ausearch {
  float_t calculate_expected_time(ausearch_node const& node) {
    return (node.success_probability * node.success_time) +
           ((1-node.success_probability) * node.failure_time);
  }

  float_t calculate_weight(std::vector<node_info> const& nodes) {
    float_t weight = 0;

    for (auto iter = nodes.rbegin(); iter != nodes.rend(); ++iter) {
      weight = (iter->node.success_probability * iter->node.success_time) +
          ((1-iter->node.success_probability) * (iter->node.failure_time + weight));
    }

    return weight;
  }
  float_t calculate_weight(std::vector<ausearch_node> const& nodes) {
    float_t weight = 0;

    for (auto iter = nodes.rbegin(); iter != nodes.rend(); ++iter) {
      weight = (iter->success_probability * iter->success_time) +
          ((1-iter->success_probability) * (iter->failure_time + weight));
    }

    return weight;
  }

  void minimise_nodes(std::vector<node_info>& nodes) {
    if (nodes.size() < 2) return;

    // First, we calculate an upper bound on the weight
    float_t weight, old_weight = calculate_weight(nodes);
    while (true) {
      float_t remaining_weight = old_weight;

      // Now, iterating down the node list, trying to find the minimising value
      std::list<node_info> node_set(nodes.begin(), nodes.end());
      for (size_t pos = 0; pos < nodes.size() - 1; ++pos) {
        auto iter = node_set.begin();

        float_t max_rem = 0.;
        decltype(iter) max;
        for (; iter != node_set.end(); ++iter) {
          float_t node_remaining_weight = remaining_weight;
          // We didn't succeed
          node_remaining_weight -= (iter->node.success_probability * iter->node.success_time);
          // Expand the rest of the weight
          node_remaining_weight /= (1-iter->node.success_probability);

          if (node_remaining_weight > max_rem) {
            max_rem = node_remaining_weight;
            max = iter;
          }
        }

        nodes[pos] = *max;
        node_set.erase(max);
        remaining_weight = max_rem;
      }
      nodes.back() = node_set.front();
      weight = calculate_weight(nodes);

      if (weight <= old_weight)
        return;
      old_weight = weight;
    }
  }

  std::vector<node_info> convert_nodes(std::vector<ausearch_node> const& input) {
    std::vector<ausearch::node_info> nodes(input.size());
    for (size_t i = 0; i < input.size(); ++i)
      nodes[i] = ausearch::node_info{.node = input[i], .index=i};
    return nodes;
  }
}
