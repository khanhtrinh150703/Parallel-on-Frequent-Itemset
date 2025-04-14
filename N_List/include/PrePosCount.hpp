#pragma once

#ifndef PREPOSCOUNT_HPP
#define PREPOSCOUNT_HPP

#include <iostream>
#include <NodePPC.hpp>
#include <list>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <chrono>
#include "ThreadPool.hpp"
using namespace std;
using namespace chrono;

class PrePosCount
{

public:
    // Attributes
    static mutex mtx;

    static map<int32_t, int32_t> countItems;
    static int32_t prev;
    map<int32_t, vector<int32_t>> dataById;
    double minsub;
    // map<int32_t, vector<int32_t>> data;
    NodePPC *root;

    // Constructor
    PrePosCount(double minsub);

    // Destructor
    ~PrePosCount();

    // Methods

    // Recursive free memory for tree
    void deleteTree(NodePPC *nodePPC);

    // Read data from text file:
    // Hàm đọc dữ liệu từ file và xử lý song song dựa trên ID
    void readDataById(const string &filename, size_t numThreads);
    // Read file single
    void readDataById(const string &filename);
    // Compare Function for item in transaction
    static bool compareByQuantity(const int32_t &a, const int32_t &b);

    // Sort itemset order by rank of countItem
    void sortByCountItem();

    // Show countItems
    static void showCountItems();
    // Show data
    void showData();
    // Show root
    void showRoot();

    // Recursive add item
    void addItemset(NodePPC *current, const vector<int32_t>& itemset);

    // BuildMulti PPC tree
    void buildMulti(size_t numThread);

    // BuildSingle PPC tree
    void buildSingle();


    // Add Prev
    // void addPre(NodePPC* current, int32_t index = 0);

    int32_t ppcMarker(NodePPC *current, int32_t post);
    void processItemsets(NodePPC *root, const map<int32_t, vector<int32_t>> &dataById, const vector<int32_t> &keys);
    void processTransaction(int32_t transactionId, const vector<int32_t> &itemset,
                            vector<pair<int32_t, vector<int32_t>>> &groupedMap);


private:
    // Hàm xử lý một nhóm giao dịch có cùng ID
    void processTransactionsById(const vector<pair<int32_t, int32_t>> &transactions, int32_t start, int32_t end, unordered_map<int32_t, vector<int32_t>> &localData, unordered_map<int32_t, int32_t> &localCountItems);
};

#endif