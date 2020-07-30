#pragma once

#include <ciphey/ausearch.swig.hxx>

namespace ciphey::ausearch {
  using edge_info = ausearch_edge const*;

  float_t calculate_weight(std::vector<edge_info> const& edges);
  float_t calculate_weight(std::vector<ausearch_edge> const& edges);

  float_t minimise_edges(std::vector<edge_info>& edges);
  std::vector<edge_info> convert_edges(std::vector<ausearch_edge> const& input);
}
