#include <ciphey/ausearch.hpp>

#include <algorithm>
#include <list>
#include <numeric>
#include <utility>

#include <iostream>

namespace ciphey::ausearch {
//  float_t calculate_expected_time(ausearch_node const& node) {
//    return (node.success_probability * node.success_time) +
//           ((1-node.success_probability) * node.failure_time);
//  }

  // Iterating forward is *way* faster
  float_t calculate_antiweight(std::vector<node_info> const& nodes) {
    return std::accumulate(nodes.begin(), nodes.end(), float_t{0.}, [](float_t weight, auto& i) {
      ausearch_node const& node = *i;
      return (node.success_probability * node.success_time) + (node.failure_probability * (node.failure_time + weight));
    });
  }

  float_t calculate_weight(std::vector<node_info> const& nodes) {
    return std::accumulate(nodes.rbegin(), nodes.rend(), float_t{0.}, [](float_t weight, auto& i) {
      ausearch_node const& node = *i;
      return (node.success_probability * node.success_time) + ((node.failure_probability) * (node.failure_time + weight));
    });
  }

  float_t calculate_weight(std::vector<ausearch_node> const& nodes) {
    return std::accumulate(nodes.rbegin(), nodes.rend(), float_t{0.}, [](float_t weight, auto& node) {
      return (node.success_probability * node.success_time) + ((node.failure_probability) * (node.failure_time + weight));
    });
  }

  float_t brute_nodes(std::vector<node_info>& nodes, size_t target) {
    float_t best_weight = calculate_antiweight(nodes);
    // Triangle swaps
    for (size_t i = target + 1; i < nodes.size(); ++i) {
      std::swap(nodes[i], nodes[target]);
      auto new_weight = calculate_antiweight(nodes);
      if (new_weight < best_weight)
        best_weight = new_weight;
      else
        // Swap back
        std::swap(nodes[i], nodes[target]);
    }

    return best_weight;
  }

  float_t minimise_nodes(std::vector<node_info>& nodes) {
    if (nodes.size() == 0)
      return std::numeric_limits<float_t>::quiet_NaN();
    else if (nodes.size() == 1)
      return calculate_weight(nodes);

    // It turns out that it is faster to optimise for antiweight (weight in reverse)
    //
    // This is because weight is iteratively calculated from the end,
    // and most machines run faster when iterating *forwards*

    // First, we calculate an upper bound on the weight
    float_t weight, old_weight = calculate_antiweight(nodes);
    size_t n = 0;
    while (true) {
      ++n;
      float_t remaining_weight = old_weight;

      // Now, iterating down the node list, trying to find the minimising value
      for (size_t pos = 0; pos < nodes.size() - 1; ++pos) {
        float_t max_rem = 0.;
        size_t max = -1;

        for (size_t i = pos; i < nodes.size(); ++i) {
          float_t node_remaining_weight = remaining_weight;
          auto& target = nodes[i];
          // We didn't succeed
          node_remaining_weight -= (target->success_probability * target->success_time);
          // Expand the rest of the weight
          node_remaining_weight /= (target->failure_probability);

          if (node_remaining_weight > max_rem) {
            max_rem = node_remaining_weight;
            max = i;
          }
        }

        std::swap(nodes[pos], nodes[max]);
        remaining_weight = max_rem;
      }
      weight = calculate_antiweight(nodes);

      while (true) {
        float_t tmp_weight = weight;

        for (size_t i = 0; i < nodes.size() - 2; ++i)
          brute_nodes(nodes, i);
        weight = brute_nodes(nodes, nodes.size() - 2);
        if (weight == tmp_weight)
          break;
        tmp_weight = weight;
      }

      if (weight == old_weight)
        break;

      old_weight = weight;
    }

    // antiweight -> weight
    std::reverse(nodes.begin(), nodes.end());
    return calculate_weight(nodes);
  }

  std::vector<node_info> convert_nodes(std::vector<ausearch_node> const& input) {
    std::vector<ausearch::node_info> nodes(input.size());
    for (size_t i = 0; i < input.size(); ++i)
      nodes[i] = &input[i];// /*.index=i*/};
    return nodes;
  }
}
