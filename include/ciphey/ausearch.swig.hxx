#include <ciphey/typedefs.hpp>

namespace ciphey {
  struct ausearch_node {
    prob_t success_probability, failure_probability; // Caching 1-success_probability
    float_t success_time, failure_time;

    ausearch_node(prob_t success_probability_, float_t success_time_, float_t failure_time_):
      success_probability{success_probability_}, success_time{success_time_}, failure_time{failure_time_} {
      failure_probability = 1-success_probability;
    }
  };
}

