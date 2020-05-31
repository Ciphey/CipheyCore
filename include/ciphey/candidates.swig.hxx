#include <ciphey/typedefs.hpp>

namespace ciphey {
  template<typename Key>
  struct crack_result {
    Key key;
    prob_t p_value;
  };
}
