#include "PPCTree.hpp"

using namespace std;
mutex mutexCount;
mutex mutexChildren;
// counting_semaphore<10> semaphore(4);

// Constructor
PPCTree::PPCTree(int32_t minsub) : minsub(minsub), countTotal(0)
{
    tuple<int32_t, int32_t, int32_t> tmpPP_count(0, 0, 0);
    root = new NodeN_List("root", tmpPP_count);
}

// Destructor
PPCTree::~PPCTree()
{
    deleteTree(root);
    delete root;
}

// Recursive free memory for tree
void PPCTree::deleteTree(NodeN_List *nodeNList)
{
    if (!nodeNList)
        return;

    // Duyệt qua các con bằng chỉ số và đệ quy xóa chúng
    for (size_t i = 0; i < nodeNList->children.size(); ++i)
    {
        deleteTree(nodeNList->children[i]); // Đệ quy xóa cây con có gốc là nodeNList->children[i]
    }

    // After deleting all children, delete the current node
    // delete nodeNList;
}

// Show root
void PPCTree::showRoot()
{
    ofstream outputFile("bin/tree_structure.txt"); // Open a file for output

    if (outputFile.is_open())
    {
        if (root)
        {
            root->show(outputFile, 0); // Start displaying from the root node at depth 0
        }
        else
        {
            cerr << "Root is not initialized!" << endl;
        }
        outputFile.close(); // Close the file after output
    }
    else
    {
        cerr << "Could not open the file!" << endl;
    }
}

// Calculate the total count in a vector of tuples
int32_t PPCTree::sum_count(const vector<tuple<int32_t, int32_t, int32_t>> &PP_code)
{
    int32_t total = 0;
    for (size_t i = 0; i < PP_code.size(); ++i)
    {
        total += get<2>(PP_code[i]);
    }
    return total;
}

// Init branchRoot
void PPCTree::initRoot(NodePPC *current)
{
    if (!current)
        return;

    // Duyệt qua các con bằng chỉ số và gọi hàm initRoot đệ quy
    for (size_t i = 0; i < current->children.size(); ++i)
    {
        initRoot(current->children[i]);
    }

    // Kiểm tra và thêm nút vào root nếu itemset khác "null"
    if (current->itemset != "null")
    {
        root->insertRoot(current->itemset, make_tuple(current->prev, current->post, current->quantity));
    }
}

void PPCTree::buildTreeMulti(NodeN_List *parent, size_t num)
{   
    // auto endTreeBuild = chrono::high_resolution_clock::now();
    if (!parent)
        return;

    size_t childCount = parent->children.size();
    if (childCount == 0)
    {
        cerr << "children null." << endl;
        return;
    }

    // Điều chỉnh num để tránh tạo thread thừa
    num = min(num, childCount);
    ThreadPool pool(num);
    vector<future<void>> futures;
    futures.reserve(childCount);

    // Hàm để thêm công việc vào pool
    auto enqueueWork = [&](size_t s, size_t e) {
        futures.emplace_back(pool.enqueue([this, parent, s, e] {
            this->process_elements(parent, s, e);
        }));
    };

    size_t midPoint = childCount / 2;
    size_t batchSize = max(size_t(1), childCount / num); // Tăng batchSize để giảm kiểm tra

    // Xử lý nửa đầu (từ 0 đến midPoint)
    size_t start = 0;
    size_t batchCount = 0;
    while (start < midPoint)
    {
        size_t end = start + 1;
        enqueueWork(start, end);
        start = end;
        ++batchCount;

        if (batchCount >= batchSize || start == midPoint)
        {
            for (auto it = futures.begin(); it != futures.end(); )
            {
                if (it->wait_for(chrono::milliseconds(0)) == future_status::ready)
                {
                    it->get();
                    it = futures.erase(it); // Dùng erase với iterator
                }
                else
                {
                    ++it;
                }
            }
            batchCount = 0;
        }
    }

    // Xử lý nửa sau (từ midPoint đến childCount)
    start = midPoint;
    batchCount = 0;
    while (start < childCount)
    {
        size_t end = start + 1;
        enqueueWork(start, end);
        start = end;
        ++batchCount;

        if (batchCount >= batchSize || start == childCount)
        {
            for (auto it = futures.begin(); it != futures.end(); )
            {
                if (it->wait_for(chrono::milliseconds(0)) == future_status::ready)
                {
                    it->get();
                    it = futures.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            batchCount = 0;
        }
    }

    // Đợi tất cả công việc còn lại
    for (auto &future : futures)
    {
        future.get();
    }
    // auto endTime = chrono::high_resolution_clock::now();
    // auto durationTime = chrono::duration_cast<chrono::microseconds>(endTime - endTreeBuild).count();
    // cout << "Time taken for buildTreeMulti: " << durationTime / 1e6 << " seconds" << endl;
}


void PPCTree::buildTreeMultiFuture(NodeN_List* parent, size_t num_threads) {
    if (!parent) {
        std::cerr << "Parent null." << std::endl;
        return;
    }

    size_t child_count = parent->children.size();
    if (child_count == 0) {
        std::cerr << "Children null." << std::endl;
        return;
    }

    // Giới hạn số thread
    num_threads = std::min(num_threads, child_count);
    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
    }

    // Tăng kích thước công việc mỗi task
    size_t task_size = std::max(size_t(1), child_count / (num_threads * 4)); // Giảm số task
    std::vector<std::future<void>> futures;
    futures.reserve(child_count / task_size + 1);

    auto enqueue_work = [this, parent](size_t start, size_t end) {
        return std::async(std::launch::async, [this, parent, start, end] {
            try {
                this->process_elements(parent, start, end);
            } catch (const std::exception& e) {
                std::cerr << "Error in async task [" << start << ", " << end << "]: " << e.what() << std::endl;
                throw;
            } catch (...) {
                std::cerr << "Unknown error in async task [" << start << ", " << end << "]" << std::endl;
                throw;
            }
        });
    };

    size_t mid_point = child_count / 2;
    size_t batch_size = std::max(size_t(1), num_threads * 2); // Batch lớn hơn
    size_t batch_count = 0;

    // Giới hạn số task đồng thời
    const size_t max_concurrent_tasks = num_threads * 4;

    // Nửa đầu
    size_t start = 0;
    while (start < mid_point) {
        size_t end = std::min(start + task_size, mid_point);
        try {
            // Chờ nếu có quá nhiều task
            while (futures.size() >= max_concurrent_tasks) {
                for (auto it = futures.begin(); it != futures.end(); ) {
                    try {
                        if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                            it->get();
                            it = futures.erase(it);
                        } else {
                            ++it;
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing future: " << e.what() << std::endl;
                        it = futures.erase(it);
                    }
                }
            }
            futures.push_back(enqueue_work(start, end));
        } catch (const std::exception& e) {
            std::cerr << "Failed to enqueue task [" << start << ", " << end << "]: " << e.what() << std::endl;
            continue; // Tiếp tục thay vì return
        } catch (...) {
            std::cerr << "Unknown error enqueuing task [" << start << ", " << end << "]" << std::endl;
            continue;
        }
        start = end;
        ++batch_count;

        if (batch_count >= batch_size || start == mid_point) {
            for (auto it = futures.begin(); it != futures.end(); ) {
                try {
                    if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                        it->get();
                        it = futures.erase(it);
                    } else {
                        ++it;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error processing future: " << e.what() << std::endl;
                    it = futures.erase(it);
                }
            }
            batch_count = 0;
        }
    }

    // Nửa sau
    start = mid_point;
    batch_count = 0;
    while (start < child_count) {
        size_t end = std::min(start + task_size, child_count);
        try {
            while (futures.size() >= max_concurrent_tasks) {
                for (auto it = futures.begin(); it != futures.end(); ) {
                    try {
                        if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                            it->get();
                            it = futures.erase(it);
                        } else {
                            ++it;
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error processing future: " << e.what() << std::endl;
                        it = futures.erase(it);
                    }
                }
            }
            futures.push_back(enqueue_work(start, end));
        } catch (const std::exception& e) {
            std::cerr << "Failed to enqueue task [" << start << ", " << end << "]: " << e.what() << std::endl;
            continue;
        } catch (...) {
            std::cerr << "Unknown error enqueuing task [" << start << ", " << end << "]" << std::endl;
            continue;
        }
        start = end;
        ++batch_count;

        if (batch_count >= batch_size || start == child_count) {
            for (auto it = futures.begin(); it != futures.end(); ) {
                try {
                    if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                        it->get();
                        it = futures.erase(it);
                    } else {
                        ++it;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error processing future: " << e.what() << std::endl;
                    it = futures.erase(it);
                }
            }
            batch_count = 0;
        }
    }

    // Đợi tất cả
    for (auto& future : futures) {
        try {
            future.get();
        } catch (const std::exception& e) {
            std::cerr << "Error in final future: " << e.what() << std::endl;
        }
    }
}

void PPCTree::process_elements(NodeN_List* parent, size_t start, size_t end) {
    const size_t num_children = parent->children.size();
    end = std::min(end, num_children);

    for (size_t i = start; i < end; ++i) {
        countTotal++; // Atomic, an toàn đa luồng

        NodeN_List* current = parent->children[i];
        if (!current) {
            std::cerr << "Node null at index " << i << std::endl;
            continue;
        }

        current->children.reserve(num_children - i - 1);

        for (size_t j = i + 1; j < num_children; ++j) {
            NodeN_List* other = parent->children[j];
            if (!other) {
                // std::cerr << "Node other null at index " << j << std::endl;
                continue;
            }

            NodeN_List* tmp = combine(current, other);
            if (tmp && sum_count(tmp->vectorPP_count) >= this->minsub) {
                current->children.push_back(tmp);
            } else {
                delete tmp;
            }
        }

        // Đệ quy tuần tự để giữ đúng logic gốc
        if (!current->children.empty()) {
            process_elements(current, 0, current->children.size());
        }
    }
}

// Tối ưu combine
NodeN_List* PPCTree::combine(NodeN_List* front, NodeN_List* behind) {
    if (!front || !behind) return nullptr;

    // Optimize string concatenation
    string combineItem = front->itemset.size() == 1 
        ? front->itemset + behind->itemset 
        : front->itemset + behind->itemset.back();

    vector<tuple<int32_t, int32_t, int32_t>> tmpListTuple;
    const auto& frontList = front->vectorPP_count;
    const auto& behindList = behind->vectorPP_count;

    // Precompute initial sum if needed, but here we track it dynamically
    double sum = 0;
    for (const auto& t : frontList) sum += get<2>(t);
    for (const auto& t : behindList) sum += get<2>(t);

    size_t i = 0, j = 0;
    double totalSum = 0;
    tmpListTuple.reserve(min(frontList.size(), behindList.size())); // Conservative estimate

    while (i < frontList.size() && j < behindList.size()) {
        const auto& itemFront = frontList[i];
        const auto& itemBehind = behindList[j];

        if (get<0>(itemBehind) < get<0>(itemFront)) {
            if (get<1>(itemBehind) > get<1>(itemFront)) {
                if (!tmpListTuple.empty() && get<0>(itemBehind) == get<0>(tmpListTuple.back())) {
                    get<2>(tmpListTuple.back()) += get<2>(itemFront);
                    totalSum += get<2>(itemFront);
                } else {
                    tmpListTuple.emplace_back(get<0>(itemBehind), get<1>(itemBehind), get<2>(itemFront));
                    totalSum += get<2>(itemFront);
                }
                i++;
            } else {
                sum -= get<2>(itemBehind);
                j++;
            }
        } else {
            sum -= get<2>(itemFront);
            i++;
        }

        if (sum < minsub) return nullptr;
    }

    return totalSum < minsub ? nullptr : new NodeN_List(combineItem, move(tmpListTuple));
}
// Đây là phiên bản bình thường
// Build the PPC tree from NodeN_List parent
void PPCTree::buildTreeSingle(NodeN_List *parent)
{   
    if (!parent)
        return;

    int32_t childCount = parent->children.size();

    for (int32_t i = 0; i < childCount; i++)
    {
        countTotal++;
        NodeN_List *current = parent->children[i];

        // Optimize inner loop by checking only necessary combinations
        for (int32_t j = i + 1; j < childCount; j++)
        {
            NodeN_List *other = parent->children[j];

            // Only create a new node if conditions are met
            NodeN_List *tmp = combine(current, other);
            if (tmp)
            {
                if (sum_count(tmp->vectorPP_count) >= minsub)
                {
                    current->children.push_back(tmp);
                }
                else
                {
                    delete tmp;
                }
            }
        }

        // Build subtree recursively
        buildTreeSingle(current);
    }
} 