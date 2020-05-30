#include <Ciphey/typedefs.hpp>

namespace Ciphey {
  template<typename Key>
  struct crack_result {
    Key key;
    prob_t p_value;
  };
}
