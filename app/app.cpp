#include<optimal-parsing/lzw.hpp>
#include<vector>
#include<set>
#include<string>
#include<cassert>
#include<iostream>
#include <optimal-parsing/dictionary_opt.hpp>

using namespace std;

#define DBG 0

std::set<char> get_alphabet(std::string const & w) {
    std::set<char> S;
    for (auto c : w) S.insert(c);
    return S;
}

auto const TEST_WORDS = std::vector<std::string> {
        // "#",
        // "#a",
        "#abababaabaabaaab",
        "#aaabbabaabaaabab",
        "#abababcccacadcadaabcada",
        "#ababcbababaa",
        "#abbbcaabbcbbcaaac",
        "#abccdeabccdeacdeacdeacde",
        "#bcbbbacbba",
        "#bbbbb"
};

auto const TEST_WORDS_LZW_OPTIMAL = std::vector<std::string> {
                                                           "#abababaabaabaaab"
                                                   };

auto const TEST_WORDS_LZ78_OPTIMAL = std::vector<std::string> {
                                                            "#abababcccacadcadaabcada"
                                                    };

class Test {
public:

    void print(std::string label, std::string const & w, std::vector<std::string> const & vec) {
        if (!DBG) return;
        std::cout << label << " for " << w << std::endl;
        for (auto x : vec) std::cout << x << ";";
        std::cout << endl;
    }

    void check_word(std::string const & w) {
        // if (DBG) std::cout << "CHECK " << w << std::endl;
        // auto alpha = get_alphabet(w);
        // auto compressor = LZWCompressor(alpha);
        auto compressed = LZWCompressor<TrieReverseTrie, OptimalOutputParser>::compress(w);
        print(std::string{"Opt"}, w, compressed);
        std::string decompressed = '#' + LZWDecompressor<TrieReverseTrie>::decompress(compressed, get_alphabet(w));
        
        assert(decompressed == w);

        auto compressed_greedy = LZWCompressor<TrieReverseTrie, GreedyOutputParser>::compress(w);
        print(std::string{"Gre"}, w, compressed_greedy);
        std::string decompressed_greedy = '#' + LZWDecompressor<TrieReverseTrie>::decompress(compressed, get_alphabet(w));
        
        assert(decompressed_greedy == w);
        assert(compressed.size() <= compressed_greedy.size());
    }

    void test_empty() {
        check_word("#");
    }

    void test_one_letter() {
        check_word("#a");
    }

    void test_hand() {
        for (auto w : TEST_WORDS) {
            check_word(w);
        }
    }

    void test_lzw_optimality() {
        for (auto w : TEST_WORDS_LZW_OPTIMAL) {
            auto compressed = LZWCompressor<TrieReverseTrie, OptimalOutputParser>::compress(w);
            auto compressed_greedy = LZWCompressor<TrieReverseTrie, GreedyOutputParser>::compress(w);

            assert(compressed.size() < compressed_greedy.size());
        }
    }

    std::string random_word(int m, std::vector<char> a) {
        std::string res;
        for (int i = 0; i < m; ++i) {
            res += a[rand()%a.size()];
        }
        return res;
    }

    void test_large_random() {
        auto m = 1000000;
//        auto A = std::vector<char>{'a', 'b', 'c'};
        auto A = std::vector<char>();
        for (int i = 0; i < 26; ++i) A.push_back(char('a' + i));
        for (int i = 0; i < 1; ++i) {
            auto w = '#' + random_word(m, A);
            check_word(w);
        }
    }
};

int main() {
    std::ios_base::sync_with_stdio(false);

    Test test;
    std::cout << "Init ok" << std::endl;
    test.test_hand();
    std::cout << "Hand ok" << std::endl;
    test.test_lzw_optimality();
    std::cout << "Optimality ok" << std::endl;
    test.test_empty();
    std::cout << "Empty ok" << std::endl;
    test.test_one_letter();
    std::cout << "One letter ok" << std::endl;
    test.test_large_random();
    std::cout << "Large random ok" << std::endl;
}
