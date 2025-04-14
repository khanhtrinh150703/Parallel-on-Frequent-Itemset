#pragma once

#ifndef NODEN_LIST_HPP
#define NODEN_LIST_HPP

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <fstream>
#include <vector>

using namespace std;

class NodeN_List
{
public:
    // Attributes
    string itemset;
    vector<tuple<int32_t, int32_t, int32_t>> vectorPP_count;

    // NodeN_vector* parent;
    vector<NodeN_List *> children;

    // Constructor
    // Constructor with one tuple
    NodeN_List(string itemset, tuple<int32_t, int32_t, int32_t> PP_count);

    // Constructor with vector of tuples
    NodeN_List(string itemset, vector<tuple<int32_t, int32_t, int32_t>> vectorPP_count);

    // Destructor
    ~NodeN_List();

    // Methods:

    // Show data NodeN_vector
    void show(ofstream &file, int32_t depth);

    // Insert root NodeN_vector
    void insertRoot(string item, tuple<int32_t, int32_t, int32_t> PP_count);

    // Insert NodeN_vector
    NodeN_List *insertNode(NodeN_List *parent, string item, vector<tuple<int32_t, int32_t, int32_t>> vectorPP_count);
};

#endif // NODEN_vector_HPP
