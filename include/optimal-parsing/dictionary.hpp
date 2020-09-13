
#pragma once
#include <map>
#include <algorithm>
#include <cassert>

class TrieDict {
public:

    std::map<char, TrieDict*> children;
    TrieDict * link;
    bool terminal;
    std::string label;
    std::string edge;
    int depth;
    int index;
    TrieDict * parent;

// public:

    TrieDict(std::string label, std::string edge = "", int depth = 0, int index = -1, TrieDict * parent = NULL) {
        this->link = NULL;
        this->terminal = false;

        this->label = label;
        this->edge = edge;
        this->depth = depth;
        this->index = index;
        
        if (parent == NULL) this->parent = this;
        else this->parent = parent;
    }

  TrieDict * insert(std::string w, std::string code, int index = -1) {
    auto current = this;
    for (auto const & c: w) {
        if (!current->children.count(c)) {
            current->children[c] = new TrieDict(std::string{"-1"}, std::string{c}, current->depth + 1, index, current);
        }
        current = current->children[c];
    }
    current->terminal = true;
    current->label = code;
    current->index = index;

    return current;
  }

  TrieDict * search(std::string const & w) {
    auto current = this;

    if (!w.size()) {
        return current;
    }
    for (auto const & c: w) {
        if (!current->children.count(c)) {
            return NULL;
        }
        current = current->children[c];
    }
    return current;
    
    // if not w:
    //   return self
    // if w[0] not in this->children:
    //   return None
    // child = this->children[w[0]]
    // return child.search(w[1:])
  }

  TrieDict * extend(char c) {
    return this->children.count(c) ? this->children[c] : NULL;
  }

  TrieDict * contract() {
    auto current = this->link->parent;
    while (current->parent != current && current->link == NULL) {
      current = current->parent;
    }
    return current->link;
  }

  void connect(TrieDict * const node) {
    this->link = node;
  }
};

class TrieReverseTrie {
public:
  TrieDict * trie;
  TrieDict * trie_rev;
  int size;
  
  TrieReverseTrie() {
      this->trie = new TrieDict("@", "", 0, 0);
      this->trie_rev = new TrieDict("@", "", 0, 0);
      this->size = 0;

      this->trie->connect(this->trie_rev);
      this->trie_rev->connect(this->trie);
  }

  TrieDict * insert(TrieDict * node, std::string w, std::string wr, std::string code) {
    this->size += 1;
    auto new_node = node->insert(w, code, this->size);
    auto new_node_rev = this->trie_rev->insert(wr, code, this->size);
    new_node->connect(new_node_rev);
    new_node_rev->connect(new_node);
    return new_node;
  }

  TrieDict * search(std::string w) {
    return this->trie->search(w);
  }

  std::string get_prefix(TrieDict * node) {
    std::string prefix = "";
    while (node->parent != node) {
      prefix += node->edge;
      node = node->parent;
    }
    std::reverse(prefix.begin(), prefix.end());
    return prefix;
  }
};
