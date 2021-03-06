#pragma once

#include<cassert>
#include<set>
#include<string>
#include<unordered_map>
#include<utility>
#include<vector>

#include "parser.hpp"

extern int DBG;

template<typename Dict, typename Po>
class LZWCompressor {
private:

    std::set<char> alphabet;
    Dict *dictionary;

    DictParser<Dict> *parser_dict;
    Po *parser_out;

    void reset() {
        dictionary = new Dict();
        int i = 0;
        for (auto c : alphabet) {
            dictionary->insert(std::string{c}, i++);
        }

        parser_dict = new DictParser<Dict>(dictionary);
        parser_out = new Po(dictionary);
    }

public:

    explicit LZWCompressor(std::set<char> alphabet) : alphabet(std::move(alphabet)) {
        reset();
    }

    int parse(char c) {
        auto code = parser_out->parse(c);
        parser_dict->parse(c);
        return code;
    }

    std::vector<int> flush() {
        auto codes = parser_out->flush();
        return codes;
    }

    static std::vector<int> compress(std::string const &w) {
        // Construct alphabet
        std::set<char> alpha;
        for (auto c : w) alpha.insert(c);
        // Construct instance of compressor and compress
        auto instance = LZWCompressor<Dict, Po>(alpha);
        auto compressed = std::vector<int>();
        for (auto c : w) {
            auto code = instance.parse(c);
            if (code != -1) {
                compressed.push_back(code);
            }
        }
        auto code = instance.flush();
        if (code.size()) {
            for (auto const &r : code) {
                compressed.push_back(r);
            }
        }

        if (DBG) {
            std::cout << "=== STATS ===" << std::endl;
            auto siz = instance.dictionary->getSize();
            auto t = w.size() - 1;
            std::cout << "Sizeofs of Trie, ReverseTrie, ptr: " << sizeof(Trie) << " " << sizeof(ReverseTrie) << " "
                      << sizeof(Trie *) << std::endl;
            std::cout << "|D| real: " << siz << std::endl;
            std::cout << "|D| theoretical estimation = O(|T|/log |T|): " << t / logl(t) << std::endl;
            std::cout << "Estimated memory taken (based on sizeofs): "
                      << (static_cast<long double>(siz * 1 * sizeof(Trie) + siz * 2 * sizeof(ReverseTrie)) / 1000000)
                      << " MB" << std::endl;
        }
        return compressed;
    }

    ~LZWCompressor() {
        delete dictionary;
        delete parser_dict;
        delete parser_out;
    }
};

template<typename Dict>
class LZWDecompressor {
private:

    std::set<char> alphabet;
    Dict *dictionary;

    DictParser<Dict> *parser_dict;
    std::unordered_map<int, Trie *> reference;

    void reset() {
        dictionary = new Dict();
        reference = std::unordered_map<int, Trie *>();
        int i = 0;
        for (auto c : alphabet) {
            reference[i] = dictionary->insert(std::string{c}, i);
            i++;
        }

        parser_dict = new DictParser<Dict>(dictionary);
    }

public:

    explicit LZWDecompressor(std::set<char> alphabet) : alphabet(std::move(alphabet)) {
        reset();
    }

    std::string parse(int const c) {
        std::string parsed;

        while (!reference.count(c)) {
            auto node = parser_dict->phrase;
            auto prefix = parser_dict->dict->get_prefix(node);

            for (auto x : prefix) {
                parsed += x;
                auto added = parser_dict->parse(x);
                if (added != nullptr) {
                    reference[added->getLabel()] = added;
                    if (added->getLabel() == c) {
                        break;
                    }
                }
            }
        }

        auto node = reference[c];
        auto phrase = parser_dict->dict->get_prefix(node);

        for (std::size_t i = parsed.size(); i < phrase.size(); ++i) {
            auto x = phrase[i];
            auto added = parser_dict->parse(x);
            if (added != nullptr) {
                reference[added->getLabel()] = added;
            }
        }

        return phrase;
    }

    static std::string decompress(std::vector<int> const &code, std::set<char> const &alphabet) {
        auto instance = LZWDecompressor<Dict>(alphabet);
        std::string ans;
        for (auto c: code) ans += instance.parse(c);
        return ans;
    }

    ~LZWDecompressor() {
        delete dictionary;
        delete parser_dict;
    }
};
