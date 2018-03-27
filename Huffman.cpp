#include "Huffman.h"
//#define PRINT
//#define PRINT_TREE

using namespace std;
//=============================================================================
bool Huffman::ByFreq::operator()(const Node* node1, const Node* node2)
{
    return node1->_frequency > node2->_frequency;
}

void Huffman::addChance(const int x, unsigned char ch)
{
    _leafs.push_back(new Node(x, ch));
}

void Huffman::buildTree()
{
    for (int i = 0; i < _leafs.size(); ++i)
        _nodes.push(_leafs[i]);

    //get two less significant nodes and merge them into one
    //then push merged node back to priority queue
    while (_nodes.size() != 1)
    {
        Node* leftNode = _nodes.top();
        _nodes.pop();
        Node* rightNode = _nodes.top();
        _nodes.pop();

        Node* mergedNode = new Node(leftNode->_frequency + rightNode->_frequency, ' ');
        mergedNode->_left = leftNode;  //less frequent node
        mergedNode->_right = rightNode;

        _nodes.push(mergedNode);
    }
    _root = _nodes.top();
    _nodes.pop();

    _root->_code = "";
    makeCode(_root, "");
}

void Huffman::makeCode(Node* node, string code)
{
    if (!node->_left)
    {
        node->_code = code;
        return;
    }
    node->_right->_bit = '0';
    node->_left->_bit = '1';
    makeCode(node->_right, code + "0");
    makeCode(node->_left, code + "1");
}

void Huffman::writeTreeToFile(const string& filename)
{
    ofstream fout;
    fout.open(filename, ios::binary);
    _root->writeSubTree(fout);
    fout.close();
}
