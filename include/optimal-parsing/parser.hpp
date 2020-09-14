#pragma once

#include<algorithm>
#include<dictionary.hpp>
#include<functional>
#include<stdexcept>
#include<iostream>
#include "lzw.hpp"

class DictParser {
public:
    TrieReverseTrie * dict;
    TrieDict * phrase;
    std::function<std::string(DictParser*, char)> label_extractor;
    std::function<TrieDict*(DictParser*, char)> extension_callback;

    DictParser(
        TrieReverseTrie * dictionary, 
        std::function<std::string(DictParser*, char)> label_extractor,
        std::function<TrieDict*(DictParser*, char)> extension_callback
    ) {
        dict = dictionary;
        phrase = dict->trie;
        this->label_extractor = label_extractor;
        this->extension_callback = extension_callback;
    }

    TrieDict * parse(char c) {
        auto extended = phrase->extend(c);
        if (extended == NULL) {
            auto reversed_phrase = dict->get_prefix(phrase) + c;
            std::reverse(reversed_phrase.begin(), reversed_phrase.end());
            auto node = dict->insert(
                phrase, std::string{c}, reversed_phrase, label_extractor(this, c)
            );
            phrase = extension_callback(this, c);
            return node;
        }
        phrase = extended;
        return NULL;
    }
};

class OutputParser {
protected:
    TrieReverseTrie * dict;
    TrieDict * phrase;
public:

    OutputParser(TrieReverseTrie * dictionary) {
        dict = dictionary;
        phrase = dict->trie;
    }

    std::string parse(char c) {
        throw std::runtime_error("Parse not implemented");
    }

    std::vector<std::string> flush() {
        throw std::runtime_error("Flush not implemented");
    }

};

class GreedyOutputParser : public OutputParser {
public:

    GreedyOutputParser(TrieReverseTrie * dictionary) : OutputParser(dictionary) {}

    std::string parse(char c) {
        auto extended = phrase->extend(c);
        if (extended == NULL) {
            auto code = phrase->label; // TODO
            phrase = dict->search(std::string{c});
            return code;
        }
        phrase = extended;
        return std::string("");
    }

    std::vector<std::string> flush() {
        auto code = parse('_'); // TODO
        auto ans = std::vector<std::string>();
        if (code != "") ans.push_back(code);
        return ans;
    }
};

class OptimalOutputParser : public OutputParser {
private:
    std::string tmp_out;
    int beginning, f_beginning, offset;

public:

    OptimalOutputParser(TrieReverseTrie * dictionary) : OutputParser(dictionary) {
        beginning = 1, f_beginning = 1, offset = 0;
        tmp_out = "";
    }

    std::string parse(char c) {
        tmp_out += c;
        auto extended = phrase->extend(c);
        if (extended == NULL) { 
            auto tmp_offset = 0;
            auto tmp_depth = phrase->depth;
            while (true) {
                auto longest_suffix = phrase->contract(); // it could also take long
                tmp_offset += phrase->depth - longest_suffix->depth;
                extended = longest_suffix->extend(c);
                if (extended == NULL) {
                    phrase = longest_suffix;
                } else {
                    phrase = extended;
                    break;
                }
            }
            if (beginning + offset + tmp_offset > f_beginning + 1) {
                auto tmp_node = dict->search(tmp_out.substr(0, offset));
                auto code = tmp_node->label;
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

  std::vector<std::string> flush() {
        auto code = std::vector<std::string>();
        if (offset > 0) {
            auto tmp_node = dict->search(tmp_out.substr(0, offset));
            code.push_back(tmp_node->label);
            tmp_out = tmp_out.substr(offset);
        }
        if (tmp_out.size()) {
            auto tmp_node = dict->search(tmp_out);
            code.push_back(tmp_node->label);
        }
        return code;
  }
};
