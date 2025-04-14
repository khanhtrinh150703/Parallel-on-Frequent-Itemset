#include "NodeN_List.hpp"
using namespace std;

// Constructor with one tuple
NodeN_List::NodeN_List(string itemset, tuple<int32_t, int32_t, int32_t> PP_count) : itemset(itemset)
{
    vectorPP_count.push_back(PP_count);
}

// Constructor with vector of tuples
NodeN_List::NodeN_List(string itemset, vector<tuple<int32_t, int32_t, int32_t>> listPP_count) : itemset(itemset), vectorPP_count(listPP_count) {}

// Destructor
NodeN_List::~NodeN_List()
{
    // Destroy vector<NodeN_List*>
    for (size_t i = 0; i < children.size(); ++i)
    {
        delete children[i];
    }
    children.clear();
}

// Method insertRoot
void NodeN_List::insertRoot(string item, tuple<int32_t, int32_t, int32_t> PP_count)
{
    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i]->itemset == item)
        {
            children[i]->vectorPP_count.push_back(PP_count);
            return;
        }
    }

    NodeN_List *nodeList = new NodeN_List(item, PP_count);
    children.push_back(nodeList);
}

// Method insertNode
NodeN_List *NodeN_List::insertNode(NodeN_List *parent, string item, vector<tuple<int32_t, int32_t, int32_t>> listPP_count)
{
    for (size_t i = 0; i < parent->children.size(); ++i)
    {
        if (parent->children[i]->itemset == item)
        {
            // Optional: Handle case where item already exists
            // (you might want to merge or update the existing node)
            return parent->children[i];
        }
    }

    NodeN_List *newNodeNList = new NodeN_List(item, listPP_count);
    parent->children.push_back(newNodeNList);
    return newNodeNList;
}

// Method show
void NodeN_List::show(ofstream &file, int32_t depth)
{
    // Display indentation based on depth
    for (int32_t i = 0; i < depth; ++i)
    {
        file << "  "; // Indentation
    }

    // Display itemset and listPP_count
    file << "Itemset: " << itemset << ", PP_Counts: [";

    bool first = true;
    for (const auto &pp : vectorPP_count)
    {
        if (!first)
        {
            file << ", ";
        }
        file << "(" << get<0>(pp) << ", " << get<1>(pp) << ", " << get<2>(pp) << ")";
        first = false;
    }
    file << "]" << endl;

    // Display child nodes
    for (auto *child : children)
    {
        child->show(file, depth + 1);
    }
}
