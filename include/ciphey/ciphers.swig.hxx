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

  namespace playfair {
    using key_t = std::vector<char_t>;
  }

  namespace vigenere {
    /// A sequence of offsets
    using key_t = std::vector<size_t>;

    struct key_len_candidate {
      size_t len;
      prob_t p_value;
      windowed_freq_table tab;
    };
    struct key_len_res {
      std::vector<key_len_candidate> candidates;
      size_t count_in_domain;
    };
  }

  namespace xor_single {
    /// A sequence of offsets
    using key_t = uint8_t;
  }

  namespace xorcrypt {
    /// A sequence of offsets
    using key_t = std::vector<uint8_t>;
  }
}
