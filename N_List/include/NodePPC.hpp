#pragma once

#ifndef NodePPC_HPP
#define NodePPC_HPP

#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <fstream>

using namespace std;

class NodePPC
{
public:
    // Attributes
    string itemset;
    int32_t quantity;
    int32_t prev;
    int32_t post;
    // NodePPC* parent;
    vector<NodePPC *> children;

    // Constructor
    NodePPC(string itemset);
    NodePPC(const std::string &itemset, int32_t quantity, int32_t prev, int32_t post);
    // Destructor
    ~NodePPC();

    // Methods:

    // Show tree: NodePPC - Left to Right
    void show(int32_t depth = 0);
    void showNodePPCtoChildren(NodePPC *current);

    // Insert NodePPC
    NodePPC *insert(NodePPC *parent, const std::string &item);

    void merge(NodePPC *otherTree);
    void append(NodePPC *otherTree);
};

#endif