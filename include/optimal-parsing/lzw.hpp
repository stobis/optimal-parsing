#pragma once

#include <set>
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>
#include <parser.hpp>
#include <dictionary.hpp>


class LZWDictParser : public DictParser {
public:
    LZWDictParser(TrieReverseTrie * dictionary) : DictParser(
        dictionary,
        [](DictParser const * d, char c) { return std::to_string(d->dict->size); },
        [](DictParser const * d, char c) { return d->dict->trie->search(std::string{c}); }
    ) {}
};

template<typename Dict, typename Po>
class LZWCompressor {
private:

    std::set<char> alphabet;
    Dict dictionary;

    LZWDictParser * parser_dict;
    Po * parser_out;

    void reset() {
        dictionary = TrieReverseTrie();
        int i = 0;
        for (auto c : alphabet) {
            auto label = std::to_string(i++);
            dictionary.insert(dictionary.trie, std::string{c}, std::string{c}, label);
        }

        parser_dict = new LZWDictParser(&dictionary);
        parser_out = new Po(&dictionary); // todo: uwaga na to czy to nie musza byc te same dicty a nie tylko copy assignment (a chyba musza, czyli daj ptry)
    }

public:

    LZWCompressor(std::set<char> const & alphabet) : alphabet(alphabet) {
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

    static std::vector<std::string> compress(std::string w) {
        // Construct alphabet
        std::set<char> alpha;
        for (auto c : w) alpha.insert(c);
        // Construct instance of compressor and compress
        auto instance = LZWCompressor<Dict, Po>(alpha);
        auto compressed = std::vector<std::string>();
        for (int i = 1; i < w.size(); ++i) {
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

    LZWDictParser * parser_dict;
    std::unordered_map<int, TrieDict*> reference;

    void reset() {
        dictionary = TrieReverseTrie();
        reference = std::unordered_map<int, TrieDict*>();
        int i = 0;
        for (auto c : alphabet) {
            auto label = std::to_string(i);
            reference[i] = dictionary.insert(dictionary.trie, std::string{c}, std::string{c}, label);
            i++;
        }

        parser_dict = new LZWDictParser(&dictionary);
    }

public:

    LZWDecompressor(std::set<char> const & alphabet) : alphabet(alphabet) {
        reset();
    }

    std::string parse(int const c) {
        auto parsed = std::string{""};

        while (!reference.count(c)) {
            auto node = parser_dict->phrase;
            auto prefix = parser_dict->dict->get_prefix(node);

            for (auto x : prefix) {
                parsed += x;
                auto added = parser_dict->parse(x);
                if (added != NULL) {
                    reference[stoi(added->label)] = added;
                    if (stoi(added->label) == c) {
                        break;
                    }
                }
            }
        }

        auto node = reference[c];
        auto phrase = parser_dict->dict->get_prefix(node);

        for (int i = parsed.size(); i < phrase.size(); ++i) {
            auto x = phrase[i];
            auto added = parser_dict->parse(x);
            if (added != NULL) {
                reference[stoi(added->label)] = added;
            }
        }

        return phrase;
    }

    static std::string decompress(std::vector<std::string> code, std::set<char> alphabet) {
        auto instance = LZWDecompressor<Dict>(alphabet);
        std::string ans = "";
        for (auto c: code) ans += instance.parse(std::stoi(c));
        return ans;
    }
};

std::set<char> get_alphabet(std::string w) {
    std::set<char> S;
    for (auto c : w) S.insert(c);
    return S;
}

// vector<string> lzw_compress(w, parser_output=parser.OptimalOutputParser) {
//     auto alphabet = get_alphabet(w);
//     instance = LZWCompressor<LZWDictParser, OptimalOutputParser>(alphabet);
//     auto compressed = vector<string>();
//     for (int i = 1; i < w.size(); ++i) {
//         auto code = instance.parse(c);
//         if (code != "") {
//             compressed.push_back(code);
//         }
//     }
//     auto code = instance.finish();
//     if (code != "") {
//         for (auto r : code) {
//             compressed.push_back(code);
//         }
//     }
//     return compressed;
// }

// string lzw_decompress(string code, set<char> alphabet) {
//     auto instance = LZWDecompressor(alphabet);
//     std::string ans = "";
//     for (auto c: code) ans += parse(int(c));
//     return ans;
// }
