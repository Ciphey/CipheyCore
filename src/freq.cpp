#include <ciphey/freq.hpp>

#include <boost/math/special_functions/gamma.hpp>

#include <set>

namespace ciphey {
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

  float_t chisq_cdf(freq_t dof, float_t up_to) {
    return boost::math::gamma_p(static_cast<float>(dof) / 2, up_to / 2);
  }


  prob_t gof_chisq(assoc_table const& assoc, freq_t count) {
    auto stat = run_chisq(assoc, count);
    // Handle the asymptopic value
    if (stat == std::numeric_limits<float_t>::infinity())
      return 0;
    // We want the upper tail
    auto p_value = 1 - chisq_cdf(count - 1, stat);
    return p_value;
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
    for (auto& i : target)
      if (auto iter = lookup.find(i.first); iter == lookup.end() || iter->second == 0)
        to_remove.push_back(i.first);

    for (auto i : to_remove)
      target.erase(i);

    return;
  }

  void freq_analysis(windowed_freq_table& tabs, string_t const& str) {
    for (size_t i = 0; i < str.size(); ++i)
      ++tabs[i % tabs.size()][str[i]];
  }

  void freq_analysis(windowed_freq_table& tabs, string_t const& str, std::set<char_t> domain) {
    size_t i = 0;
    for (auto& c : str) {
      if (domain.count(c)) {
        ++tabs[i % tabs.size()][c];
        ++i;
      }
    }
  }
}

