#pragma once

#include <ciphey/ausearch.swig.hxx>

namespace ciphey::ausearch {
  struct node_info_old {
    ausearch_node const*  node;
    size_t index;
  };
  using node_info = ausearch_node const*;

  float_t calculate_weight(std::vector<node_info> const& nodes);
  float_t calculate_weight(std::vector<ausearch_node> const& nodes);

  float_t minimise_nodes(std::vector<node_info>& nodes);
  std::vector<node_info> convert_nodes(std::vector<ausearch_node> const& input);
}
