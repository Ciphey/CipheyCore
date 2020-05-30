#include <Ciphey/typedefs.hpp>

namespace Ciphey {
  template<typename Key>
  struct crack_results {
    Key key;
    prob_t p_value;
  };
}
