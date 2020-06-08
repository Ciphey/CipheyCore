#include <ciphey/typedefs.hpp>

namespace ciphey {
  using group_t = std::vector<char_t>;
  using inverse_group_t = std::map<char_t, size_t>;

  template<typename Key>
  struct crack_result {
    Key key;
    prob_t p_value;
  };
  namespace caesar {
    using key_t = size_t;
  }

  namespace vigenere {
    /// A sequence of offsets
    using key_t = std::vector<size_t>;
  }
}
