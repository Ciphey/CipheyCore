#include <ciphey/freq.hpp>

#include <boost/math/special_functions/gamma.hpp>

#include <future>
#include <optional>
#include <random>
#include <set>
#include <thread>

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

  prob_t chisq_cdf(freq_t dof, float_t up_to) {
    // Handle the asymptopic value first, to save time
    if (up_to == std::numeric_limits<float_t>::infinity())
      return 1;
    if (up_to == 0)
      return 0;
    return boost::math::gamma_p(static_cast<float>(dof) / 2, up_to / 2);
  }

  prob_t gof_test(assoc_table const& assoc, freq_t count) {
    auto stat = run_chisq(assoc, count);
    // We want the upper tail
    auto p_value = 1 - chisq_cdf(assoc.size() - 1, stat);
    return p_value;
  }

  assoc_table create_assoc_table(prob_table const& observed, prob_table const& expected) {
    // TODO: optimise
    //
    // First, let's get all the keys
    struct key_elem {
      std::optional<float_t> obs_val, exp_val;
    };

    std::map<char_t, key_elem> keys;
    for (auto& i : expected)
      keys[i.first].exp_val = i.second;
    // We can use index notation for this, as that gets default constructed
    for (auto& i : observed)
      keys[i.first].obs_val = i.second;

    // We can now fill in all the values, with non-existent expected values being defined as zero
    assoc_table ret;
    ret.reserve(keys.size());

    for (auto& i : keys)
      ret.emplace_back(assoc_table_elem{
        .observed = i.second.obs_val.value_or(0),
        .expected = i.second.exp_val.value_or(0)});

    return ret;
  }

  prob_table freq_conv(freq_table const& freqs, freq_t total_len) {
    prob_table ret;
    for (auto& i : freqs)
      ret[i.first] = static_cast<prob_t>(i.second) / total_len;
    return ret;
  }
  prob_table freq_conv(freq_table const& freqs) {
    freq_t total_len = 0;
    for (auto& i : freqs)
      total_len += i.second;
    return freq_conv(freqs, total_len);
  }
  windowed_prob_table freq_conv(windowed_freq_table& freqs, freq_t total_len) {
    windowed_prob_table ret;
    ret.reserve(total_len);
    for (auto& i : freqs)
      // TODO: work the length out here, rather than do slow counting
      ret.emplace_back(freq_conv(i));
    return ret;
  }
  windowed_prob_table freq_conv(windowed_freq_table& freqs) {
    freq_t total_len = 0;
    for (auto& i : freqs)
      for (auto& entry: i)
        total_len += entry.second;
    return freq_conv(freqs, total_len);
  }

  size_t filter_missing(freq_table& target, prob_table const& lookup) {
    std::vector<char_t> to_remove;
    size_t ret = 0;
    for (auto& i : target) {
      if (auto iter = lookup.find(i.first); iter == lookup.end() || iter->second == 0) {
        to_remove.push_back(i.first);
        ret += i.second;
      }
    }

    for (auto i : to_remove)
      target.erase(i);

    return ret;
  }

  size_t filter_missing(freq_table& target, domain_t const& tab) {
    std::vector<char_t> to_remove;
    size_t ret = 0;
    for (auto& i : target) {
      if (!tab.count(i.first)) {
        to_remove.push_back(i.first);
        ret += i.second;
      }
    }

    for (auto i : to_remove)
      target.erase(i);

    return ret;
  }

//  size_t filter_missing(windowed_freq_table& target, prob_table const& lookup) {
//    size_t acc = 0;
//    for (auto& i : target)
//      acc += filter_missing(i, lookup);
//    return acc;
//  }

  void freq_analysis(freq_table& tab, string_t const& str) {
    for (auto& i : str)
      ++tab[i];
  }
  size_t freq_analysis(freq_table& tab, string_t const& str, domain_t const& domain) {
    size_t n = 0;
    for (auto& i : str) {
      if (domain.count(i)) {
        ++tab[i];
        ++n;
      }
    }
    return n;
  }
  void freq_analysis(windowed_freq_table& tabs, string_t const& str, size_t offset) {
    for (size_t i = 0; i < str.size(); ++i)
      ++tabs[(offset + i) % tabs.size()][str[i]];
  }

  size_t freq_analysis(windowed_freq_table& tabs, string_t const& str, domain_t const& domain, size_t offset) {
    for (auto& c : str) {
      if (domain.count(c)) {
        ++tabs[offset % tabs.size()][c];
        ++offset;
      }
    }
    return offset;
  }

  string_t generate_fuzz(prob_table const& tab, size_t len) {
    string_t ret;
    ret.resize(len);
    std::mt19937 rng;

    {
      thread_local std::random_device seed_rng;
      thread_local std::uniform_int_distribution<decltype(rng)::result_type> seed_dist;
      rng.seed(seed_dist(seed_rng));
    }

    thread_local std::uniform_real_distribution<float_t> dist{0, 1};

    for (auto& rand_char : ret) {
    restat_char:
      float_t stat = dist(rng);
      // Iterate through the table, removing the probabilities until we are within a bracket
      for (auto const& i : tab) {
        if ((stat -= i.second) <= 0) {
          rand_char = i.first;
          goto next_char;
        }
      }
      // This should not happen in normal usage!
      //
      // However, floats do weird things with rounding, so we will be leniant
      goto restat_char;
      next_char: {}
    }

    return ret;
  }

  assoc_table closeness_assoc(prob_table const& observed, prob_table const& expected) {
    // What can we possibly do with this?
    if (observed.size() == 0)
      return {{.observed = 0, .expected = 1}};

    assoc_table assoc;
    assoc.reserve(expected.size());
    for (auto& i : expected)
      assoc.emplace_back(assoc_table_elem{.expected = i.second});
    std::sort(assoc.rbegin(), assoc.rend(),
              [](assoc_table_elem& a, assoc_table_elem& b) { return a.expected < b.expected; });

    std::vector<prob_t> observed_sorted;
    observed_sorted.reserve(observed.size());
    for (auto& i : observed)
      observed_sorted.emplace_back(i.second);
    std::sort(observed_sorted.rbegin(), observed_sorted.rend());
    // Trim unobserved values
    while (observed_sorted.back() == 0) {
      observed_sorted.pop_back();
      if (observed.size() == 0)
        return {{.observed = 0, .expected = 1}};
    };

    // Fill table with observed values, or zeroes where appropriate
    size_t i;
    if (observed.size() > expected.size()) {
      assoc.resize(observed.size());
      for (i = 0; i < expected.size(); ++i)
        assoc[i].observed = observed_sorted[i];
      for (; i < observed.size(); ++i)
        assoc[i] = assoc_table_elem{.observed = observed_sorted[i], .expected = 0};
    }
    else {
      for (i = 0; i < observed.size(); ++i)
        assoc[i].observed = observed_sorted[i];
      for (; i < expected.size(); ++i)
        assoc[i].observed = 0;
    }

    return assoc;
  }

  prob_t closeness_test(prob_table const& observed, prob_table const& expected, freq_t count) {
    // Quick bypass to avoid more filling than we have to
    if (observed.size() > expected.size())
      return 0;

    auto assoc = closeness_assoc(observed, expected);
    auto stat = run_chisq(assoc, count);

    return 1 - chisq_cdf(assoc.size() - 1, stat);
  }

  prob_t closeness_test(windowed_prob_table const& observed, prob_table const& expected, freq_t count) {
    struct imdt_res_t {
      size_t tab_size;
      float_t chi_sq;
    };

    std::vector<std::future<imdt_res_t>> asyncs(observed.size());

    for (size_t i = 0; i < observed.size(); ++i) {
      asyncs[i] = std::async(std::launch::async, [&, i]() -> imdt_res_t {
        auto assoc = closeness_assoc(observed[i], expected);

        // We can normalise at the end for efficiency
        return {.tab_size = assoc.size(), .chi_sq = run_chisq(assoc)};
      });
    }

    float_t stat = 0;
    size_t total_tab_len = 0;
    for (auto& i : asyncs) {
      auto res = i.get();
      total_tab_len += res.tab_size;
      stat += res.chi_sq;
    }

    // Finally, we can normalise
    stat *= count;

    return 1 - chisq_cdf(total_tab_len - 1, stat);
  }

  float_t calculate_entropy(std::map<uint8_t, freq_t> const& freqs, size_t len) {
    float_t ret = 0;
    for (auto& i : freqs)  {
      float_t observed_prob = static_cast<float_t>(i.second) / len;
      ret -= observed_prob * ::log2(observed_prob);
      // Some nice printing, kept for future debug
//      std::cout << "  " << std::hex << (int)i.first << ": " << i.second << std::endl;
    }

//    std::cout << ret << std::endl;
    return ret;
  }

  float_t information_content(data const& b) {
    // TODO:
    // I would prefer something more empirical,
    // rather than just brute forcing the token size

    std::map<uint8_t, freq_t> byte_freqs;
    std::map<uint8_t, freq_t> nybble_freqs;
    std::map<uint8_t, freq_t> pair_freqs;
    std::map<uint8_t, freq_t> bit_freqs;
    for (auto i : b) {
      ++byte_freqs[i];
      ++nybble_freqs[i>>4];
      ++nybble_freqs[i&0xf];
      ++pair_freqs[i&0x3];
      ++pair_freqs[(i>>2)&0x3];
      ++pair_freqs[(i>>4)&0x3];
      ++pair_freqs[(i>>6)&0x3];
      for (int bit = 0; bit < 8; ++bit)
        ++bit_freqs[(i >> bit) & 1];
    }

    return std::min({
                      calculate_entropy(byte_freqs,   b.size()    ) / 8,
                      calculate_entropy(nybble_freqs, b.size() * 2) / 4,
                      calculate_entropy(pair_freqs,   b.size() * 4) / 2,
                      calculate_entropy(bit_freqs,    b.size() * 8)
                    }) * b.size();
  }
}

