#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>

class Trie;

class ReverseTrie;

class TrieReverseTrie;

class Trie {
private:

    std::unordered_map<char, Trie *> children;
    Trie const *parent = nullptr;
    ReverseTrie const *link = nullptr;
    int label{};
    int depth;
    char edge;

public:

    Trie(const Trie *parent, int depth, char edge) : parent(parent), depth(depth), edge(edge) {}

    Trie *insert(std::string const &w, int const &code) {
        auto current = this;
        for (auto const &c: w) {
            if (!current->children.count(c)) {
                current->children[c] = new Trie(current, current->depth + 1, c);
            }
            current = current->children[c];
        }
        current->label = code;

        return current;
    }

    Trie *search(std::string const &w) {
        auto current = this;

        if (w.empty()) {
            return current;
        }
        for (auto const &c: w) {
            if (!current->children.count(c)) {
                return nullptr;
            }
            current = current->children[c];
        }
        return current;
    }

    Trie *extend(char c) {
        return this->children.count(c) ? this->children[c] : nullptr;
    }

    // Get & set
    const std::unordered_map<char, Trie *> &getChildren() const {
        return children;
    }

    void setChildren(const std::unordered_map<char, Trie *> &children) {
        Trie::children = children;
    }

    const Trie *getParent() const {
        return parent;
    }

    void setParent(const Trie *parent) {
        Trie::parent = parent;
    }

    const ReverseTrie *getLink() const {
        return link;
    }

    void setLink(const ReverseTrie *link) {
        Trie::link = link;
    }

    int getLabel() const {
        return label;
    }

    void setLabel(int label) {
        Trie::label = label;
    }

    int getDepth() const {
        return depth;
    }

    void setDepth(int depth) {
        Trie::depth = depth;
    }

    char getEdge() const {
        return edge;
    }

    void setEdge(char edge) {
        Trie::edge = edge;
    }

    ~Trie() {
        for (auto &child : children) {
            delete child.second;
        }
    }
};

class ReverseTrie {
private:

    std::unordered_map<char, ReverseTrie *> children;
    ReverseTrie const *parent = nullptr;
    Trie const *link = nullptr;
    Trie const *lower_bound = nullptr;
    Trie const *upper_bound = nullptr;

public:

    ReverseTrie(const ReverseTrie *parent, const Trie *lower_bound, const Trie *upper_bound) : parent(parent),
                                                                                               lower_bound(lower_bound),
                                                                                               upper_bound(
                                                                                                       upper_bound) {}

    ReverseTrie *insert(Trie const *const start, Trie const *const end) {
        auto const start_label = start->getEdge();
        if (!children.count(start_label)) {
            // Needs totally new edge
            children[start_label] = new ReverseTrie(this, start, end);
            return children[start_label];
        }
        // Will go through some edge
        auto const child_node = children[start_label];

        const Trie *T_lower_ptr = start;
        auto const T_upper_ptr = end;
        auto RT_lower_ptr = child_node->getLower_bound();
        auto const RT_upper_ptr = child_node->getUpper_bound();

        while (T_lower_ptr != T_upper_ptr && RT_lower_ptr != RT_upper_ptr) {
            auto const T_label = T_lower_ptr->getEdge();
            auto const RT_label = RT_lower_ptr->getEdge();
            if (T_label == RT_label) {
                // Everything is ok, go further
                T_lower_ptr = T_lower_ptr->getParent();
                RT_lower_ptr = RT_lower_ptr->getParent();
            } else {
                // We have to split
                auto *internal = new ReverseTrie(this, child_node->getLower_bound(), RT_lower_ptr);
                auto *side = new ReverseTrie(internal, T_lower_ptr, T_upper_ptr);
                child_node->setLower_bound(RT_lower_ptr);
                child_node->setParent(internal);

                internal->setChild(T_label, side);
                internal->setChild(RT_label, child_node);
                setChild(start_label, internal);

                return side;
            }
        }
        // Edge cases
        if (T_lower_ptr == T_upper_ptr && RT_lower_ptr == RT_upper_ptr) {
            // Edge is precisely covered
            return child_node;
        } else if (T_lower_ptr == T_upper_ptr) {
            // I'm covered, but in RT there are a few chars left (so i become the new internal node)
            auto *internal = new ReverseTrie(this, start, T_upper_ptr);
            child_node->setLower_bound(RT_lower_ptr);
            child_node->setParent(internal);

            internal->setChild(RT_lower_ptr->getEdge(), child_node);
            setChild(start_label, internal);

            return internal;
        } else {
            // There are some letters left in out word to insert, recursive call should do the work
            return child_node->insert(T_lower_ptr, T_upper_ptr);
        }
    }

    // Get & set
    const std::unordered_map<char, ReverseTrie *> &getChildren() const {
        return children;
    }

    void setChildren(const std::unordered_map<char, ReverseTrie *> &children) {
        ReverseTrie::children = children;
    }

    const Trie *getLower_bound() const {
        return lower_bound;
    }

    void setLower_bound(const Trie *lower_bound) {
        ReverseTrie::lower_bound = lower_bound;
    }

    const Trie *getUpper_bound() const {
        return upper_bound;
    }

    void setUpper_bound(const Trie *upper_bound) {
        ReverseTrie::upper_bound = upper_bound;
    }

    void setChild(char c, ReverseTrie *child) {
        children[c] = child;
    }

    const ReverseTrie *getParent() const {
        return parent;
    }

    void setParent(const ReverseTrie *parent) {
        ReverseTrie::parent = parent;
    }

    const Trie *getLink() const {
        return link;
    }

    void setLink(const Trie *link) {
        ReverseTrie::link = link;
    }

    ~ReverseTrie() {
        for (auto &child : children) {
            delete child.second;
        }
    }
};

class TrieReverseTrie {
private:
    Trie *trie;
    ReverseTrie *trie_rev;
    int size;

public:

    TrieReverseTrie() {
        trie = new Trie(nullptr, 0, 0);
        trie_rev = new ReverseTrie(nullptr, nullptr, nullptr);
        size = 0;

        trie->setLink(trie_rev);
        trie_rev->setLink(trie);
    }

    Trie *insert(std::string const &w, int const &code) {
        return insert(trie, w, code);
    }

    Trie *insert(Trie *const node, std::string const &w, int const &code) {
        size += 1;
        auto new_node = node->insert(w, code);
        auto new_node_rev = trie_rev->insert(new_node, trie);
        new_node->setLink(new_node_rev);
        new_node_rev->setLink(new_node);
        return new_node;
    }

    Trie *search(std::string const &w) {
        return trie->search(w);
    }

    std::string get_prefix(Trie const *node) {
        std::string prefix;
        while (node->getParent() != nullptr) {
            prefix += node->getEdge();
            node = node->getParent();
        }
        std::reverse(prefix.begin(), prefix.end());
        return prefix;
    }

    Trie *getTrie() const {
        return trie;
    }

    ReverseTrie *getTrie_rev() const {
        return trie_rev;
    }

    int getSize() const {
        return size;
    }

    virtual ~TrieReverseTrie() {
        delete trie;
        delete trie_rev;
    }
};

Trie *contract(Trie *node) {
    auto current = node->getLink()->getParent();
    while (current->getParent() != current && current->getLink() == nullptr) {
        current = current->getParent();
    }
    return const_cast<Trie *>(current->getLink());
}
