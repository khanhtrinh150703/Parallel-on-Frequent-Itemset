#include <PrePosCount.hpp>

map<int32_t, int32_t> PrePosCount::countItems;
int32_t PrePosCount::prev = 0;
mutex PrePosCount::mtx;

// Constructor
PrePosCount::PrePosCount(double minSub) : minsub(minSub)
{
    root = new NodePPC("null");
}

// Destructor
PrePosCount::~PrePosCount()
{
    countItems.clear();
    dataById.clear();
    deleteTree(root);
    delete root;
}

// Methods

// Recursive free memory for tree
void PrePosCount::deleteTree(NodePPC *nodePPC)
{
    if (nodePPC == nullptr)
        return;

    // Duyệt qua các con bằng chỉ số và gọi hàm deleteTree đệ quy
    for (size_t i = 0; i < nodePPC->children.size(); ++i)
    {
        deleteTree(nodePPC->children[i]); // Đệ quy xóa cây con
    }

    // nodePPC->~NodePPC();
}

// Hàm xử lý các giao dịch từ một khối dữ liệu
void PrePosCount::processTransactionsById(const vector<pair<int32_t, int32_t>> &transactions, int32_t start, int32_t end, unordered_map<int32_t, vector<int32_t>> &localData, unordered_map<int32_t, int32_t> &localCountItems)
{
    for (int32_t i = start; i < end; ++i)
    {
        int32_t transactionId = transactions[i].first;
        int32_t itemKey = transactions[i].second;

        // Cập nhật đếm item cục bộ
        localCountItems[itemKey]++;

        // Thêm item vào giao dịch có cùng ID
        localData[transactionId].push_back(itemKey);
    }
}

// Hàm đọc file và xử lý song song dựa trên ID
void PrePosCount::readDataById(const string &filename, size_t numThreads)
{
    ifstream file(filename);

    // Kiểm tra nếu không mở được file
    string line;
    int numberOfTransactions;
    // Read the first line to get the number of transactions
    if (getline(file, line))
    {
        istringstream iss(line);
        iss >> numberOfTransactions;
        minsub = minsub * numberOfTransactions;
        // cout<< minsub << endl;
    }
    else
    {
        cerr << "File is empty or failed to read the number of transactions!" << endl;
        file.close();
        return;
    }

    vector<pair<int32_t, int32_t>> transactions;

    // Đọc tất cả các dòng từ file
    while (getline(file, line))
    {
        istringstream iss(line);
        int32_t transactionId, itemKey;

        // Đọc từng giao dịch
        if (!(iss >> transactionId >> itemKey))
        {
            cerr << "Lỗi đọc dòng: " << line << endl;
            continue;
        }

        // Lưu trữ giao dịch dưới dạng (ID, item)
        transactions.emplace_back(transactionId, itemKey);
    }

    file.close();

    // Nếu file rỗng hoặc không đọc được giao dịch, báo lỗi
    if (transactions.empty())
    {
        cerr << "File rỗng hoặc không có giao dịch!" << endl;
        return;
    }

    // Chia các giao dịch theo số lượng luồng được chỉ định
    int32_t transactionsPerThread = transactions.size() / numThreads;
    vector<thread> threads;

    // Mỗi luồng sẽ có dữ liệu cục bộ riêng
    vector<unordered_map<int32_t, vector<int32_t>>> allLocalData(numThreads);
    vector<unordered_map<int32_t, int32_t>> allLocalCountItems(numThreads);

    // Khởi tạo các luồng để xử lý từng phần của giao dịch
    // Giả sử numThreads là kiểu size_t
    for (size_t i = 0; i < numThreads; ++i)
    {
        size_t start = i * transactionsPerThread;                                                   // Vị trí bắt đầu
        size_t end = (i == numThreads - 1) ? transactions.size() : (i + 1) * transactionsPerThread; // Vị trí kết thúc

        // Tạo luồng xử lý các giao dịch từ start đến end
        threads.emplace_back(&PrePosCount::processTransactionsById, this, ref(transactions), start, end, ref(allLocalData[i]), ref(allLocalCountItems[i]));
    }

    // Chờ tất cả các luồng kết thúc
    for (auto &t : threads)
    {
        t.join(); // Đợi luồng hoàn thành
    }

    // Hợp nhất kết quả từ tất cả các luồng
    lock_guard<mutex> lock(mtx); // Khóa để tránh truy cập đồng thời
    for (size_t i = 0; i < numThreads; ++i)
    {
        // Hợp nhất dữ liệu giao dịch theo ID
        for (const auto &entry : allLocalData[i])
        {
            dataById[entry.first].insert(dataById[entry.first].end(), entry.second.begin(), entry.second.end());
        }

        // Hợp nhất số lượng item
        for (const auto &item : allLocalCountItems[i])
        {
            countItems[item.first] += item.second; // Cập nhật số lượng item
        }
    }
}

// Read file single
void PrePosCount::readDataById(const string &filename)
{
    ifstream file(filename);

    // Check if the file was opened successfully
    string line;
    int32_t numberOfTransactions;

    // Read the first line to get the number of transactions
    if (getline(file, line))
    {
        istringstream iss(line);
        iss >> numberOfTransactions;
        minsub = minsub * numberOfTransactions;
    }
    else
    {
        cerr << "File is empty or failed to read the number of transactions!" << endl;
        file.close();
        return;
    }

    vector<pair<int32_t, int32_t>> transactions;

    // Read all lines from the file
    while (getline(file, line))
    {
        istringstream iss(line);
        int32_t transactionId, itemKey;

        // Read each transaction
        if (!(iss >> transactionId >> itemKey))
        {
            cerr << "Error reading line: " << line << endl;
            continue;
        }

        // Store the transaction as (ID, item)
        transactions.emplace_back(transactionId, itemKey);
    }

    file.close();

    // If the file is empty or no transactions were read, report an error
    if (transactions.empty())
    {
        cerr << "File is empty or no transactions found!" << endl;
        return;
    }

    // Process all transactions in a single thread
    // for (const auto &transaction : transactions)
    // {
    //     int32_t transactionId = transaction.first;
    //     int32_t itemKey = transaction.second;

    //     // Here you would include your logic for processing the transaction
    //     // For example:
    //     dataById[transactionId].push_back(itemKey);
    //     countItems[itemKey]++;
    // }

    for (size_t i = 0; i < transactions.size(); ++i)
    {
        int32_t transactionId = transactions[i].first;
        int32_t itemKey = transactions[i].second;

        // Logic xử lý transaction
        dataById[transactionId].push_back(itemKey);
        countItems[itemKey]++;
    }
}

// Show countItems
void PrePosCount::showCountItems()
{
    for (const auto &pair : countItems)
    {
        // cout << "Item: " << char(pair.first) << ", Count: " << pair.second << endl;
        cout << "Item: " << (pair.first) << ", Count: " << pair.second << endl;
    }
    cout << endl;
}

// show data
void PrePosCount::showData()
{
    // In dữ liệu
    for (const auto &entry : dataById)
    {
        cout << "Transaction ID " << entry.first << ": ";
        for (int32_t item : entry.second)
        {
            cout << item << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// Show root
void PrePosCount::showRoot()
{
    // root->showNodePPCtoChildren(root);
    root->show();
}

// Compare Function for item in transaction
bool PrePosCount::compareByQuantity(const int32_t &a, const int32_t &b)
{
    // So sánh theo số lượng trước (sắp xếp giảm dần)
    if (PrePosCount::countItems[a] > PrePosCount::countItems[b])
        return true;
    if (PrePosCount::countItems[a] < PrePosCount::countItems[b])
        return false;

    // Nếu số lượng bằng nhau, so sánh theo giá trị phần tử (sắp xếp giảm dần)
    return a > b; // So sánh giá trị phần tử, lớn hơn thì xếp trước

    // Day là ban đầu
    //  return PrePosCount::countItems[a] > PrePosCount::countItems[b];
}

// Sort itemset order by rank of countItem
void PrePosCount::sortByCountItem()
{
    // Duyệt qua các phần tử của dataById bằng chỉ số
    auto it = PrePosCount::dataById.begin();
    for (size_t i = 0; it != PrePosCount::dataById.end(); ++it, ++i)
    {
        auto &itemList = it->second;

        // Loại bỏ các item có số lượng nhỏ hơn minCountValue
        itemList.erase(
            remove_if(itemList.begin(), itemList.end(), [this](const double &itemId)
                      { return this->countItems.at(itemId) < this->minsub; }),
            itemList.end());

        // Sắp xếp các item còn lại theo số lượng
        sort(itemList.begin(), itemList.end(), compareByQuantity);
    }
}

void PrePosCount::addItemset(NodePPC *current, const vector<int32_t> &itemset)
{
    if (itemset.empty()) // Kiểm tra nếu itemset rỗng, thì dừng
        return;

    // Lấy phần tử đầu tiên của itemset và chuyển thành chuỗi
    string item = to_string(itemset.front());

    // Thêm phần tử vào cây
    NodePPC *temp = current->insert(current, item);

    // Tạo vector còn lại chứa các phần tử ngoài phần tử đầu tiên
    vector<int32_t> remainingItems(itemset.begin() + 1, itemset.end());

    // Gọi đệ quy để thêm các phần tử còn lại vào cây
    addItemset(temp, remainingItems);
}

// Build PPC tree

void PrePosCount::buildSingle()
{
    // auto start = chrono::high_resolution_clock::now(); // Start timing
    vector<int32_t> itemset;
    // Duyệt qua PrePosCount::dataById theo chỉ số
    auto it = PrePosCount::dataById.begin();
    for (size_t i = 0; it != PrePosCount::dataById.end(); ++it, ++i)
    {
        // Lấy dữ liệu từ iterator
        const auto &entry = *it;

        // Xóa dữ liệu cũ của itemset
        itemset.clear();

        // Lọc và sao chép các phần tử thỏa mãn điều kiện vào itemset
        copy_if(entry.second.begin(), entry.second.end(), back_inserter(itemset),
                [this](int32_t x)
                {
                    return countItems.at(x) >= this->minsub; // Chỉ lấy khi countitem[x] >= minsub
                });

        // Nếu itemset không rỗng, sắp xếp và thêm vào cây
        if (!itemset.empty())
        {
            // Sắp xếp itemset dựa trên hàm compareByQuantity
            sort(itemset.begin(), itemset.end(), compareByQuantity);

            // Thêm itemset vào cây
            addItemset(root, itemset);
        }
    }

    // auto end = chrono::high_resolution_clock::now(); // End timing

    // // Calculate duration in seconds
    // chrono::duration<double> duration = end - start;
    // cout << "Time taken: " << duration.count() << " seconds" << endl;
}

void PrePosCount::buildMulti(size_t numThread)
{
    vector<NodePPC*> threadRoots(numThread, nullptr);
    vector<future<void>> futures;

    // Initialize thread-local roots
    for (size_t i = 0; i < numThread; ++i) {
        threadRoots[i] = new NodePPC("threadRoot" + to_string(i));
    }

    // Divide dataById into chunks
    size_t chunkSize = dataById.size() / numThread;
    size_t remainder = dataById.size() % numThread;
    auto it = dataById.begin();

    // Process chunks in parallel using async
    for (size_t i = 0; i < numThread; ++i) {
        size_t currentChunkSize = chunkSize + (i < remainder ? 1 : 0);
        auto startIt = it;
        advance(it, currentChunkSize);
        auto endIt = it;

        futures.emplace_back(std::async(std::launch::async, 
            [this, threadRoot = threadRoots[i], startIt, endIt]() {
                vector<int32_t> itemset;
                for (auto entryIt = startIt; entryIt != endIt; ++entryIt) {
                    itemset.clear();
                    const auto& items = entryIt->second;

                    // Filter items based on minsub
                    copy_if(items.begin(), items.end(), back_inserter(itemset),
                            [this](int32_t x) { return countItems.at(x) >= this->minsub; });

                    if (!itemset.empty()) {
                        // Sort and add to thread-local root
                        sort(itemset.begin(), itemset.end(), compareByQuantity);
                        addItemset(threadRoot, itemset);
                    }
                }
            }));
    }

    // Wait for all futures to complete
    for (auto& future : futures) {
        future.get();
    }

    // Merge thread-local trees into main root
    for (auto threadRoot : threadRoots) {
        root->merge(threadRoot);
        delete threadRoot;
    }
}

// void PrePosCount::buildMulti(size_t numThread)
// {
//     ThreadPool pool(numThread);
//     vector<future<void>> futures;
    
//     // Create a temporary root node for each thread
//     vector<NodePPC*> threadRoots(numThread, nullptr);
//     for (int i = 0; i < numThread; i++) {
//         threadRoots[i] = new NodePPC("threadRoot" + to_string(i));
//     }
    
//     // Calculate the chunk size for each thread
//     size_t chunkSize = dataById.size() / numThread;
//     size_t remainder = dataById.size() % numThread;
    
//     // Process data in parallel chunks
//     auto it = dataById.begin();
//     for (size_t i = 0; i < numThread; ++i) {
//         size_t currentChunkSize = chunkSize + (i < remainder ? 1 : 0);
//         auto startIt = it;
        
//         // Advance iterator for next thread
//         advance(it, currentChunkSize);
//         auto endIt = it;
        
//         // Process this chunk in a separate thread
//         futures.emplace_back(pool.enqueue([this, threadRoot = threadRoots[i], startIt, endIt]() {
//             for (auto entryIt = startIt; entryIt != endIt; ++entryIt) {
//                 const auto &entry = *entryIt;
                
//                 // Use the same logic as in buildSingle
//                 vector<int32_t> itemset;
//                 copy_if(entry.second.begin(), entry.second.end(), back_inserter(itemset),
//                         [this](int32_t x) {
//                             return countItems.at(x) >= this->minsub;
//                         });
                
//                 if (!itemset.empty()) {
//                     // Sort the itemset just like in buildSingle
//                     sort(itemset.begin(), itemset.end(), compareByQuantity);
                    
//                     // Add to the thread-local root
//                     addItemset(threadRoot, itemset);
//                 }
//             }
//         }));
//     }
    
//     // Wait for all threads to complete
//     for (auto &future : futures) {
//         future.get();
//     }
    
//     // Merge all thread-local trees into the main root
//     for (auto threadRoot : threadRoots) {
//         root->merge(threadRoot);
//         delete threadRoot;
//     }
// }

int32_t PrePosCount::ppcMarker(NodePPC *current, int32_t post)
{
    current->prev = prev++;

    for (size_t i = 0; i < current->children.size(); ++i)
    {
        auto &child = current->children[i]; // Truy cập phần tử theo chỉ số
        post = ppcMarker(child, post);
    }

    current->post = post++;
    return post;
}
