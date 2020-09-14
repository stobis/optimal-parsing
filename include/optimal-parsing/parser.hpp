#include <utility>

#pragma once

#include<algorithm>
#include<dictionary.hpp>
#include<functional>
#include<stdexcept>
#include<iostream>
#include "lzw.hpp"
#include "dictionary_opt.hpp"

class DictParser {
public:
    TrieReverseTrie * dict;
    Trie * phrase;
    std::function<std::string(DictParser*)> label_extractor;
    std::function<Trie*(DictParser*, char)> extension_callback;

    DictParser(
        TrieReverseTrie * dictionary, 
        std::function<std::string(DictParser*)> label_extractor,
        std::function<Trie*(DictParser*, char)> extension_callback
    ) {
        dict = dictionary;
        phrase = dict->getTrie();
        this->label_extractor = std::move(label_extractor);
        this->extension_callback = std::move(extension_callback);
    }

    Trie * parse(char c) {
        auto extended = phrase->extend(c);
        if (extended == nullptr) {
            auto node = dict->insert(phrase, std::string{c}, std::to_string(dict->getSize()));
            phrase = dict->getTrie()->search(std::string{c});
            return node;
        }
        phrase = extended;
        return nullptr;
    }
};

class OutputParser {
protected:
    TrieReverseTrie * dict;
    Trie * phrase;
public:

    explicit OutputParser(TrieReverseTrie * dictionary) {
        dict = dictionary;
        phrase = dict->getTrie();
    }

    virtual std::string parse(char) = 0;

    virtual std::vector<std::string> flush() = 0;
};

class GreedyOutputParser : public OutputParser {
public:

    explicit GreedyOutputParser(TrieReverseTrie * dictionary) : OutputParser(dictionary) {}

    std::string parse(char c) override {
        auto extended = phrase->extend(c);
        if (extended == nullptr) {
            auto code = phrase->getLabel();
            phrase = dict->search(std::string{c});
            return code;
        }
        phrase = extended;
        return std::string("");
    }

    std::vector<std::string> flush() override {
        auto code = parse('_'); // TODO
        auto ans = std::vector<std::string>();
        if (!code.empty()) ans.push_back(code);
        return ans;
    }
};

class OptimalOutputParser : public OutputParser {
private:
    std::string tmp_out;
    int beginning, f_beginning, offset;

public:

    explicit OptimalOutputParser(TrieReverseTrie * dictionary) : OutputParser(dictionary) {
        beginning = 1, f_beginning = 1, offset = 0;
        tmp_out = "";
    }

    std::string parse(char c) override {
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
            return std::string();
        }
        phrase = extended;

        return std::string();
    }

  std::vector<std::string> flush() override {
        auto code = std::vector<std::string>();
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
