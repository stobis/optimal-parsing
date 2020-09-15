#pragma once

#include<algorithm>
#include<functional>
#include<iostream>
#include<stdexcept>
#include<utility>

#include "lzw.hpp"
#include "dictionary_opt.hpp"

template<typename Dict>
class DictParser {
public:
    TrieReverseTrie *dict;
    Trie *phrase;

    explicit DictParser(Dict *dictionary) {
        dict = dictionary;
        phrase = dict->getTrie();
    }

    Trie *parse(char c) {
        auto extended = phrase->extend(c);
        if (extended == nullptr) {
            auto node = dict->insert(phrase, std::string{c}, dict->getSize());
            phrase = dict->getTrie()->search(std::string{c});
            return node;
        }
        phrase = extended;
        return nullptr;
    }
};

class OutputParser {
protected:
    TrieReverseTrie *dict;
    Trie *phrase;
public:

    explicit OutputParser(TrieReverseTrie *dictionary) {
        dict = dictionary;
        phrase = dict->getTrie();
    }

    virtual int parse(char) = 0;

    virtual std::vector<int> flush() = 0;

    virtual ~OutputParser() {}
};

class GreedyOutputParser : public OutputParser {
public:

    explicit GreedyOutputParser(TrieReverseTrie *dictionary) : OutputParser(dictionary) {}

    int parse(char c) override {
        auto extended = phrase->extend(c);
        if (extended == nullptr) {
            auto code = phrase->getLabel();
            phrase = dict->search(std::string{c});
            return code;
        }
        phrase = extended;
        return -1;
    }

    std::vector<int> flush() override {
        auto code = parse('\0');
        auto ans = std::vector<int>();
        if (code != -1) ans.push_back(code);
        return ans;
    }
};

class OptimalOutputParser : public OutputParser {
private:
    std::string tmp_out;
    int beginning, f_beginning, offset;

public:

    explicit OptimalOutputParser(TrieReverseTrie *dictionary) : OutputParser(dictionary) {
        beginning = 1, f_beginning = 1, offset = 0;
        tmp_out = "";
    }

    int parse(char c) override {
        tmp_out += c;
        auto extended = phrase->extend(c);
        if (extended == nullptr) {

            auto tmp_offset = 0;
            auto tmp_depth = phrase->getDepth();
            while (true) {
                auto longest_suffix = contract(phrase); //phrase->contract(); // it could also take long

                tmp_offset += phrase->getDepth() - longest_suffix->getDepth();
                extended = longest_suffix->extend(c);
                if (extended == nullptr) {
                    phrase = longest_suffix;
                } else {
                    phrase = extended;
                    break;
                }
            }

            if (beginning + offset + tmp_offset > f_beginning + 1) {
                auto tmp_node = dict->search(tmp_out.substr(0, offset));
                auto code = tmp_node->getLabel();
                tmp_out = tmp_out.substr(offset);

                beginning += offset;
                f_beginning = beginning + tmp_depth - 1;
                offset = tmp_offset;
                return code;
            }
            offset += tmp_offset;
            return -1;
        }
        phrase = extended;

        return -1;
    }

    std::vector<int> flush() override {
        auto code = std::vector<int>();
        if (offset > 0) {
            auto tmp_node = dict->search(tmp_out.substr(0, offset));
            code.push_back(tmp_node->getLabel());
            tmp_out = tmp_out.substr(offset);
        }
        if (!tmp_out.empty()) {
            auto tmp_node = dict->search(tmp_out);
            code.push_back(tmp_node->getLabel());
        }
        return code;
    }
};
