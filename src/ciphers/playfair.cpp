#include "ciphey/ciphers.hpp"
#include "ciphey/swig.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <vector>

namespace ciphey::playfair {
    prob_t detect(prob_table const &observed, domain_t const &domain, string_ref_t str) {
        // Ciphertext is not playfair if it is not divisible by 2.
        if (str.size() % 2 != 0)
            return 0.;

        // TODO: Ciphertext is not playfair if is contains a repeated digraph (AA, BB, CC, etc.)

        // TODO: Ciphertext is not playfair if is contains more than 25 distinct letters.
        // if (observed.size() > 25) {
        //     return 0.;
        // }

        // For now, we can ignore ciphertext with non-Latin characters.
        for (auto& c : observed) {
            if (!std::binary_search(domain.begin(), domain.end(), c.first)) {
                return 0.; // Found a non-Latin character.
            }
        }

        return 0.15; // Arbitrary
    }

    key_t gen_random_key() {
        std::random_device rd;
        std::mt19937 gen(rd());
        // TODO: Use ciphertext to guess the 26th letter.
        std::vector<char_t> alphabet {
            'a', 'b', 'c', 'd', 'e',
            'f', 'g', 'h', 'i', 'k',
            'l', 'm', 'n', 'o', 'p',
            'q', 'r', 's', 't', 'u',
            'v', 'w', 'x', 'y', 'z'
        };

        std::shuffle(alphabet.begin(), alphabet.end(), gen);

        return alphabet;
    }

    string_t decrypt(string_ref_t str, key_t const& ktable) {
        int count_digraphs = str.size() / 2;

        string_t ptext = "";
        for (int i = 0; i < count_digraphs; i++) {
            char_t a = str[i * 2];
            char_t b = str[i * 2 + 1];
            int a_i = std::distance(ktable.begin(), std::find(ktable.begin(), ktable.end(), a));
            int b_i = std::distance(ktable.begin(), std::find(ktable.begin(), ktable.end(), b));

            // Same column, shift up.
            if (a_i % 5 == b_i % 5) {
                if (a_i / 5 == 0) {
                    ptext += ktable[a_i % 5 + 20];
                } else {
                    ptext += ktable[a_i - 5];
                }

                if (b_i / 5 == 0) {
                    ptext += ktable[b_i % 5 + 20];
                } else {
                    ptext += ktable[b_i - 5];
                }

            // Same row, shift left.
            } else if (a_i / 5 == b_i / 5) {
                if (a_i % 5 == 0) {
                    ptext += ktable[a_i + 4];
                } else {
                    ptext += ktable[a_i - 1];
                }

                if (b_i % 5 == 0) {
                    ptext += ktable[b_i + 4];
                } else {
                    ptext += ktable[b_i - 1];
                }

            // Rectangle, swap corners, same rows.
            } else {
                ptext += ktable[a_i / 5 * 5 + b_i % 5];
                ptext += ktable[b_i / 5 * 5 + a_i % 5];
            }
        }

        return ptext;
    }

    key_t random_swap(key_t const& key) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> a_rng(0, 24);
        std::uniform_int_distribution<> b_rng(0, 23);
        key_t new_key = key;

        auto a_i = a_rng(gen);
        auto b_i = b_rng(gen);

        // b_i cannot equal a_i for this to be a swap.
        if (a_i == b_i) {
            b_i += 1;
        }

        new_key[b_i] = key[a_i];
        new_key[a_i] = key[b_i];

        return new_key;
    }

    std::vector<crack_result<key_t>> crack(prob_table const &observed, prob_table const &expected,
                                           domain_t domain, string_ref_t str, freq_t count, prob_t p_value) {
        std::vector<crack_result<key_t>> ret {};
        key_t ktable = gen_random_key();
        string_t ptext = decrypt(str, ktable);
        auto analysis = ciphey::analyse_string(ptext, domain);
        prob_table prob_tab = ciphey::freq_conv(analysis->freqs);
        prob_t key_p_value = gof_test(create_assoc_table(prob_tab, expected), count);

        // Threshold Acceptance schedule
        int r_max = 100000;
        for (int r = 0; r < r_max; r++) {
            int timeout = 0; // After 300 random swaps we will have covered the majority of the space for the current key.
            prob_t new_score = 0;
            while (key_p_value < p_value * ((prob_t)r / (prob_t)r_max) && timeout < 300) {
                key_t new_key = random_swap(ktable);
                ptext = decrypt(str, new_key);
                analysis = ciphey::analyse_string(ptext, domain);
                prob_tab = ciphey::freq_conv(analysis->freqs);
                new_score = gof_test(create_assoc_table(prob_tab, expected), count);
                timeout += 1;

                if (new_score >= key_p_value) {
                    ktable = new_key;
                    key_p_value = new_score;
                    timeout = 0;
                }
            }

            std::cout << key_p_value << '\n';
        }

        ret.push_back({.key = ktable, .p_value = key_p_value });
        
        for (auto i : ktable) {
            std::cout << i;
        }
        std::cout << '\n';

        sort_crack_result(ret);

        return ret;
    }
}
