#include <utility>

#include <utility>

#pragma once

#include <set>
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>
#include <parser.hpp>
#include <dictionary.hpp>

template <typename Dict>
class LZWDictParser : public DictParser {
public:
    explicit LZWDictParser(Dict * dictionary) : DictParser(
        dictionary,
        [](DictParser const * d) { return std::to_string(d->dict->getSize()); },
        [](DictParser const * d, char c) { return d->dict->getTrie()->search(std::string{c}); }
    ) {}
};

template<typename Dict, typename Po>
class LZWCompressor {
private:

    std::set<char> alphabet;
    Dict dictionary;

    LZWDictParser<Dict> * parser_dict;
    Po * parser_out;

    void reset() {
        dictionary = Dict();
        int i = 0;
        for (auto c : alphabet) {
            auto label = std::to_string(i++);
            dictionary.insert(std::string{c}, label);
        }

        parser_dict = new LZWDictParser<Dict>(&dictionary);
        parser_out = new Po(&dictionary);
    }

public:

    explicit LZWCompressor(std::set<char> alphabet) : alphabet(std::move(alphabet)) {
        reset();
    }

    std::string parse(char c) {
        auto code = parser_out->parse(c);
        parser_dict->parse(c);
        return code;
    }

    std::vector<std::string> flush() {
        auto codes = parser_out->flush();
        reset();
        return codes;
    }

    static std::vector<std::string> compress(std::string const & w) {
        // Construct alphabet
        std::set<char> alpha;
        for (auto c : w) alpha.insert(c);
        // Construct instance of compressor and compress
        auto instance = LZWCompressor<Dict, Po>(alpha);
        auto compressed = std::vector<std::string>();
        for (std::size_t i = 1; i < w.size(); ++i) {
            auto code = instance.parse(w[i]);
            if (code != "") {
                compressed.push_back(code);
            }
        }
        auto code = instance.flush();
        if (code.size()) {
            for (auto const & r : code) {
                compressed.push_back(r);
            }
        }
        return compressed;
    }
};

template<typename Dict>
class LZWDecompressor {
private:

    std::set<char> alphabet;
    Dict dictionary;

    LZWDictParser<Dict> * parser_dict;
    std::unordered_map<int, Trie*> reference;

    void reset() {
        dictionary = Dict();
        reference = std::unordered_map<int, Trie*>();
        int i = 0;
        for (auto c : alphabet) {
            auto label = std::to_string(i);
            reference[i] = dictionary.insert(std::string{c}, label);
            i++;
        }

        parser_dict = new LZWDictParser<Dict>(&dictionary);
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
                    reference[stoi(added->getLabel())] = added;
                    if (stoi(added->getLabel()) == c) {
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
                reference[stoi(added->getLabel())] = added;
            }
        }

        return phrase;
    }

    static std::string decompress(std::vector<std::string> const & code, std::set<char> const & alphabet) {
        auto instance = LZWDecompressor<Dict>(alphabet);
        std::string ans = "";
        for (auto c: code) ans += instance.parse(std::stoi(c));
        return ans;
    }
};
