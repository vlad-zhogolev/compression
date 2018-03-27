#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include <vector>
#include <queue>
#include "Node.h"

class Huffman {
public:

    struct ByFreq {
        bool operator()(const Node* node1, const Node* node2);
    };

public:

    /**
     * Adds symbol to create prefix code
     * @param x - frequency of symbol
     * @param ch - symbol
     */
    void addChance(int x, unsigned char ch);

    /**
     * Creates code tree
     */
    void buildTree();

    /**
     * Returns code for symbol with index i
     * @param i - index of symbol
     * @return - code of symbol
     */
    std::string get(int i) { return _leafs[i]->_code; }

    /**
     * Stores code tree to file
     * @param filename - path to file for code tree
     */
    void writeTreeToFile(const std::string& filename);

    ~Huffman() { delete _root; }

private:

    /**
     * Recursively creates codes
     * @param node - current node in code tree
     * @param code - code of the current node
     */
    void makeCode(Node* node, std::string code);

private:
    Node* _root = nullptr;
    std::vector<Node*> _leafs;  //stores pointers to _leafs
    std::priority_queue<Node*, std::vector<Node*>, ByFreq> _nodes;
};

#endif //HUFFMAN_HUFFMAN_H
