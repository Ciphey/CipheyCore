#include <Ciphey/freq.hpp>

#include <boost/math/special_functions/gamma.hpp>

#include <set>

namespace Ciphey {
  float_t run_chisq(assoc_table const& assoc, freq_t count) {
    prob_t chisq = 0;

    for (auto const& elem : assoc) {
      if (elem.expected == 0)
        return std::numeric_limits<float_t>::infinity();

      prob_t contrib = elem.expected - elem.observed;
      contrib *= contrib;
      contrib /= elem.expected;

      chisq += contrib;
    }

    // (f_e/n - f_o/n)^2/(f_e/n) = (1/n) (f_e - f_o)/(f_e)
    //
    // We need to normalise this for our stat to be interpretable
    chisq *= count;
    return chisq;
  }

  float_t critical_chisq(freq_t dof, prob_t p_value) {
    return 2 * boost::math::gamma_q_inv(static_cast<float>(dof) / 2, p_value);
  }

  bool gof_chisq(assoc_table const& assoc, freq_t count, prob_t p_value) {
    auto stat = run_chisq(assoc, count);
    if (stat == std::numeric_limits<float_t>::infinity())
      return false;

    auto critical = critical_chisq(count - 1, p_value);
    return stat <= critical;
  }

  assoc_table create_assoc_table(prob_table const& observed, prob_table const& expected) {
    // First, let's get all the keys
    std::set<char_t> keys;
    for (auto& i : observed)
      keys.emplace(i.first);
    for (auto& i : expected)
      keys.emplace(i.first);

    // We can now fill in all the values, with non-existent expected values being defined as zero
    assoc_table ret;
    ret.reserve(keys.size());

    for (auto& i : observed) {
      ret.emplace_back(assoc_table_elem{.name = i.first, .observed = i.second});
      if (auto iter = expected.find(i.first); iter != expected.end())
        ret.back().expected = iter->second;
      else
        ret.back().expected = 0;
    }

    return ret;
  }

  void freq_analysis(freq_table& tab, string_t const& str) {
    for (auto& i : str)
      ++tab[i];
  }
  void freq_conv(prob_table& tab, freq_table const& freqs, freq_t total_len) {
    for (auto& i : freqs)
      tab[i.first] = static_cast<prob_t>(i.second) / total_len;
  }
  void freq_conv(prob_table& tab, freq_table const& freqs) {
    freq_t total_len = 0;
    for (auto& i : freqs)
      total_len += i.second;
    freq_conv(tab, freqs, total_len);
  }

  void filter_missing(prob_table& target, prob_table const& lookup) {
    std::vector<char_t> to_remove;
    for (auto& i : target) {
      if (!lookup.count(i.first))
        to_remove.push_back(i.first);
    }

    for (auto i : to_remove)
      target.erase(i);

    return;
  }
}

