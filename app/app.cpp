#include<cassert>
#include<iostream>
#include<set>
#include<string>
#include<vector>

#include "optimal-parsing/lzw.hpp"

using namespace std;

int DBG = 0; // 0 - none, 1 - stats, 2 - full

std::set<char> get_alphabet(std::string const &w) {
    std::set<char> S;
    for (auto c : w) S.insert(c);
    return S;
}

auto const TEST_WORDS = std::vector<std::string>{
        "",
        "a",
        "abababaabaabaaab",
        "aaabbabaabaaabab",
        "abababcccacadcadaabcada",
        "ababcbababaa",
        "abbbcaabbcbbcaaac",
        "abccdeabccdeacdeacdeacde",
        "bcbbbacbba",
        "bbbbb"
};

auto const TEST_WORDS_LZW_OPTIMAL = std::vector<std::string>{"abababaabaabaaab"};

class Test {
public:

    void print(const std::string &label, std::string const &w, std::vector<int> const &vec) {
        if (DBG < 2) return;
        std::cout << label << " for " << w << std::endl;
        for (auto x : vec) std::cout << x << ";";
        std::cout << endl;
    }

    void check_word(std::string const &w) {
        auto compressed = LZWCompressor<TrieReverseTrie, OptimalOutputParser>::compress(w);

        print(std::string{"Opt"}, w, compressed);
        std::string decompressed = LZWDecompressor<TrieReverseTrie>::decompress(compressed, get_alphabet(w));

        assert(decompressed == w);

        auto compressed_greedy = LZWCompressor<TrieReverseTrie, GreedyOutputParser>::compress(w);
        print(std::string{"Gre"}, w, compressed_greedy);
        std::string decompressed_greedy = LZWDecompressor<TrieReverseTrie>::decompress(compressed, get_alphabet(w));

        assert(decompressed_greedy == w);
        assert(compressed.size() <= compressed_greedy.size());
    }

    void test_empty() {
        check_word("");
    }

    void test_one_letter() {
        check_word("a");
    }

    void test_hand() {
        for (const auto &w : TEST_WORDS) {
            check_word(w);
        }
    }

    void test_lzw_optimality() {
        for (const auto &w : TEST_WORDS_LZW_OPTIMAL) {
            auto compressed = LZWCompressor<TrieReverseTrie, OptimalOutputParser>::compress(w);
            auto compressed_greedy = LZWCompressor<TrieReverseTrie, GreedyOutputParser>::compress(w);

            assert(compressed.size() < compressed_greedy.size());
        }
    }

    std::string random_word(int m, std::vector<char> a) {
        std::string res;
        for (int i = 0; i < m; ++i) {
            res += a[rand() % a.size()];
        }
        return res;
    }

    void test_large_random() {
        srand(time(NULL));

        auto m = 100000;
        auto A = std::vector<char>();
        for (int i = 0; i < 26; ++i) A.push_back(char('a' + i));
        for (int i = 0; i < 10; ++i) {
            auto w = random_word(m, A);
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
