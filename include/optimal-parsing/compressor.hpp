
#pragma once

// template<typename DictionaryParser, typename OutputParser>
// class Compressor {
// private:

//     DictionaryParser parser_dictionary;
//     OutputParser parser_output;

// public:
//     Compressor(TrieReverseTrie dictionary) {
//         this->parser_dictionary = DictionaryParser(dictionary);
//         this->parser_output = OutputParser(dictionary);
//     }

//     std::string parse(char c) {
//         this->parser_dictionary.parse(c);
//         return this->parser_output.parse(c);
//     }

//     std::string finish() {
//         return this->parser_output.parse_end();
//     }
// };

// template<typename DictionaryParser>
// class Decompressor {
// private:

//     std::map<char, char> reference;
//     DictionaryParser parser_dictionary;

// public:

//     Decompressor(TrieReverseTrie dictionary) {
//         this->reference = std::map<char, char>();
//         this->parser_dictionary = DictionaryParser(dictionary);
//     }

//     std::string parse(char c) {
//         throw new NotImplementedError();
//     }
// };
