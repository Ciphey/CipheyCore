#include <ciphey/ausearch.hpp>

#include <algorithm>
#include <list>
#include <numeric>
#include <utility>

#include <iostream>

namespace std {
  template class std::vector<ciphey::ausearch::edge_info>;
}

namespace ciphey::ausearch {


//  float_t calculate_expected_time(ausearch_edge const& edge) {
//    return (edge.success_probability * edge.success_time) +
//           ((1-edge.success_probability) * edge.failure_time);
//  }

  // Iterating forward is *way* faster
  float_t calculate_antiweight(std::vector<edge_info> const& edges) {
    return std::accumulate(edges.begin(), edges.end(), float_t{0.}, [](float_t weight, auto& i) {
      ausearch_edge const& edge = *i;
      return (edge.success_probability * edge.success_time) + (edge.failure_probability * (edge.failure_time + weight));
    });
  }

  float_t calculate_weight(std::vector<edge_info> const& edges) {
    return std::accumulate(edges.rbegin(), edges.rend(), float_t{0.}, [](float_t weight, auto& i) {
      ausearch_edge const& edge = *i;
      return (edge.success_probability * edge.success_time) + ((edge.failure_probability) * (edge.failure_time + weight));
    });
  }

  float_t calculate_weight(std::vector<ausearch_edge> const& edges) {
    return std::accumulate(edges.rbegin(), edges.rend(), float_t{0.}, [](float_t weight, auto& edge) {
      return (edge.success_probability * edge.success_time) + ((edge.failure_probability) * (edge.failure_time + weight));
    });
  }

  float_t brute_edges(std::vector<edge_info>& edges, size_t target) {
    float_t best_weight = calculate_antiweight(edges);
    // Triangle swaps
    for (size_t i = target + 1; i < edges.size(); ++i) {
      std::swap(edges[i], edges[target]);
      auto new_weight = calculate_antiweight(edges);
      if (new_weight < best_weight)
        best_weight = new_weight;
      else
        // Swap back
        std::swap(edges[i], edges[target]);
    }

    return best_weight;
  }

  float_t minimise_edges(std::vector<edge_info>& edges) {
    if (edges.size() == 0)
      return std::numeric_limits<float_t>::quiet_NaN();
    else if (edges.size() == 1)
      return calculate_weight(edges);

    // It turns out that it is faster to optimise for antiweight (weight in reverse)
    //
    // This is because weight is iteratively calculated from the end,
    // and most machines run faster when iterating *forwards*

    // First, we calculate a lower bound on the weight
    float_t weight, old_weight = calculate_antiweight(edges);
    size_t n = 0;
    while (true) {
      ++n;
      float_t remaining_weight = old_weight;

      // Now, iterating down the edge list, trying to find the minimising value
      for (size_t pos = 0; pos < edges.size() - 1; ++pos) {
        float_t max_rem = -std::numeric_limits<float_t>::infinity();
        size_t max = -1;

        for (size_t i = pos; i < edges.size(); ++i) {
          float_t edge_remaining_weight = remaining_weight;
          auto& target = edges[i];
          // We didn't succeed
          edge_remaining_weight -= (target->success_probability * target->success_time);
          // Expand the rest of the weight
          if (target->failure_probability == 0)
            // If we cannot fail, then there can be no remaining antiweight
            edge_remaining_weight = 0;
          else
            edge_remaining_weight /= (target->failure_probability);

          if (edge_remaining_weight > max_rem) {
            max_rem = edge_remaining_weight;
            max = i;
          }
        }

        std::swap(edges[pos], edges[max]);
        remaining_weight = max_rem;
      }
      weight = calculate_antiweight(edges);

      // Stop ausearch going off on one
      if (edges.size() < 20) {
        while (true) {
          float_t tmp_weight = weight;

          for (size_t i = 0; i < edges.size() - 2; ++i)
            brute_edges(edges, i);
          weight = brute_edges(edges, edges.size() - 2);
          if (weight == tmp_weight)
            break;
          tmp_weight = weight;
        }

        if (weight == old_weight)
          break;

        old_weight = weight;
      }
    }

    // antiweight -> weight
    std::reverse(edges.begin(), edges.end());
    return calculate_weight(edges);
  }

  std::vector<edge_info> convert_edges(std::vector<ausearch_edge> const& input) {
    std::vector<ausearch::edge_info> edges(input.size());
    for (size_t i = 0; i < input.size(); ++i)
      edges[i] = &input[i];// /*.index=i*/};
    return edges;
  }
}
