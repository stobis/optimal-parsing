//
//#pragma once
//#include <map>
//#include <algorithm>
//#include <cassert>
//#include <iostream>
//
//class Trie {
//public:
//
//    std::map<char, Trie*> children;
//    Trie * link;
////    bool terminal;
//    std::string label;
//    std::string edge;
//    int depth;
////    int index;
//    Trie * parent;
//
//// public:
//
//    Trie(std::string label, std::string edge = "", int depth = 0, int index = -1, Trie * parent = NULL) {
//        this->link = NULL;
////        this->terminal = false;
//
//        this->label = label;
//        this->edge = edge;
//        this->depth = depth;
////        this->index = index;
//
//        if (parent == NULL) this->parent = this;
//        else this->parent = parent;
//    }
//
//  Trie * insert(std::string const & w, std::string const & code, int index = -1) {
//    auto current = this;
//    for (auto const & c: w) {
//        if (!current->children.count(c)) {
//            current->children[c] = new Trie(std::string{"-1"}, std::string{c}, current->depth + 1, index, current);
//        }
//        current = current->children[c];
//    }
////    current->terminal = true;
//    current->label = code;
////    current->index = index;
//
//    return current;
//  }
//
//  Trie * search(std::string const & w) {
//    auto current = this;
//
//    if (!w.size()) {
//        return current;
//    }
//    for (auto const & c: w) {
//        if (!current->children.count(c)) {
//            return NULL;
//        }
//        current = current->children[c];
//    }
//    return current;
//
//    // if not w:
//    //   return self
//    // if w[0] not in this->children:
//    //   return None
//    // child = this->children[w[0]]
//    // return child.search(w[1:])
//  }
//
//  Trie * extend(char c) {
//    return this->children.count(c) ? this->children[c] : NULL;
//  }
//
//  Trie * contract() {
//    auto current = this->link->parent;
//    while (current->parent != current && current->link == NULL) {
//      current = current->parent;
//    }
//    return current->link;
//  }
//
//  void connect(Trie * const node) {
//    this->link = node;
//  }
//};
//
//class TrieReverseTrie {
//public:
//  Trie * trie;
//  Trie * trie_rev;
//  int size;
//
//  TrieReverseTrie() {
//      this->trie = new Trie("@", "", 0, 0);
//      this->trie_rev = new Trie("@", "", 0, 0);
//      this->size = 0;
//
//      this->trie->connect(this->trie_rev);
//      this->trie_rev->connect(this->trie);
//  }
//
//  Trie * insert(Trie * const node, std::string const & w, std::string const & wr, std::string const & code) {
//    this->size += 1;
//    auto new_node = node->insert(w, code, this->size);
//    auto new_node_rev = this->trie_rev->insert(wr, code, this->size); // it's probably time-consuming
//    new_node->connect(new_node_rev);
//    new_node_rev->connect(new_node);
//    return new_node;
//  }
//
//  Trie * search(std::string const & w) {
//    return this->trie->search(w);
//  }
//
//  std::string get_prefix(Trie * node) {
//    std::string prefix = "";
//    while (node->parent != node) {
//      prefix += node->edge;
//      node = node->parent;
//    }
//    std::reverse(prefix.begin(), prefix.end());
//    return prefix;
//  }
//};
