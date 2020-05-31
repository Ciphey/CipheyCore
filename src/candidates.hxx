#include <ciphey/candidates.hpp>

namespace ciphey {
  template<typename Iter, typename Core>
  Iter get_next_candidate(Iter begin, Iter end, bool(*c)(Iter const&)) {
    for (; begin != end; ++begin) {
      if (c(begin))
        return begin;
    }
    return end;
  }
}
