#pragma once

#ifndef PPCTREE_HPP
#define PPCTREE_HPP

#include <algorithm> // For algorithms like sort
#include <chrono>    // For time-related functions
#include <fstream>   // For file I/O
#include <future>    // For asynchronous operations
#include <iostream>  // For input/output
#include <map>       // For std::map
#include <mutex>     // For mutexes
#include <string>    // For std::string
#include <thread>    // For threading
#include <tuple>     // For std::tuple
#include <utility>   // For std::pair and std::move
#include <vector>    // For std::vector


// Forward declarations to avoid circular dependencies
class NodeN_List;
class NodePPC;

#include "NodeN_List.hpp"
#include "NodePPC.hpp"
#include "ThreadPool.hpp"

class PPCTree
{
public:
    // Attributes
    NodeN_List *root;
    int32_t minsub;
    std::atomic<int32_t> countTotal;
    static constexpr int MAX_POOL_DEPTH = 3; // Maximum depth for thread pool recursion
    std::atomic<int> pool_depth{0}; 
    
    // Constructor
    PPCTree(int32_t minsub);

    // Destructor
    ~PPCTree();

    // Methods

    // Initialize BranchRoot
    void initRoot(NodePPC *current);

    // Show Root
    void showRoot();

    // Combine two NodeN_List
    NodeN_List *combine(NodeN_List *front, NodeN_List *behind);

    // Sum of counts (PP_count)
    int32_t sum_count(const std::vector<std::tuple<int32_t, int32_t, int32_t>> &PP_code);

    // Build PPCTree
    void buildTreeMulti(NodeN_List *parent, size_t numThread);
    void buildTreeMultiFuture(NodeN_List* parent, size_t num_threads);
    void buildTreeSingle(NodeN_List *parent);

    // Delete tree
    void deleteTree(NodeN_List *root);
    void process_elements(NodeN_List *parent, size_t start, size_t end);
    private:
};

#endif // PPCTREE_HPP
