#include <NodePPC.hpp>

// Constructor
NodePPC::NodePPC(string itemset) : itemset(itemset), quantity(1), prev(-1), post(-1) {}

NodePPC::NodePPC(const std::string &itemset, int32_t quantity, int32_t prev, int32_t post)
    : itemset(itemset), quantity(quantity), prev(prev), post(post) {}

// Destructor
NodePPC::~NodePPC()
{
    // Destroy list<NodePPC*>
    for (size_t i = 0; i < children.size(); ++i)
    {
        delete children[i];
    }
    children.clear();
}

// Methods:

// Show tree: NodePPC - Left to Right
void NodePPC::show(int32_t depth)
{
    // Open file for writing inside the function
    ofstream outFile("output/outputShow.txt", ios::app); // Open in append mode

    // Check if the file opened successfully
    if (!outFile)
    {
        cerr << "Error opening file for writing!" << endl;
        return;
    }

    // Write the node information to the file with indentation based on depth
    for (int32_t d = 0; d < depth; d++)
    {
        outFile << "  "; // Half-tab follow depth
    }
    outFile << itemset << ": " << quantity << "{" << prev << ", " << post << "}" << endl;

    // Recursively write sub-branches to the file
    for (auto child : children)
    {
        child->show(depth + 1);
    }

    // Close file after writing
    outFile.close();
}

void NodePPC::showNodePPCtoChildren(NodePPC *current)
{
    cout << "Itemset: " << current->itemset << ", Quantity: " << current->quantity << endl;
    if (!current->children.empty())
    {
        for (auto child : current->children)
        {
            showNodePPCtoChildren(child);
        }
    }
    return;
}

// Insert NodePPC
NodePPC *NodePPC::insert(NodePPC *parent, const std::string &item)
{
    // Tìm trong children để kiểm tra xem node đã tồn tại chưa
    for (size_t i = 0; i < parent->children.size(); ++i)
    {
        // So sánh bằng toán tử ==
        if (parent->children[i]->itemset == item)
        {
            // Nếu đã tồn tại, tăng quantity và trả về node đó
            parent->children[i]->quantity++;
            return parent->children[i];
        }
    }

    // Nếu không tìm thấy node phù hợp, tạo node mới và thêm vào children
    NodePPC *newNodePPC = new NodePPC(item);
    parent->children.push_back(newNodePPC);
    return newNodePPC;
}

void NodePPC::merge(NodePPC *otherTree)
{   
    for (size_t i = 0; i < otherTree->children.size(); ++i)
    {
        bool found = false;
        // Duyệt qua các con của cây hiện tại
        for (size_t j = 0; j < this->children.size(); ++j)
        {
            if (this->children[j]->itemset == otherTree->children[i]->itemset)
            {
                // Nếu itemset giống nhau, cộng dồn quantity
                this->children[j]->quantity += otherTree->children[i]->quantity;
                found = true;
                // Gọi hàm merge đệ quy để thêm các con của child
                this->children[j]->merge(otherTree->children[i]); // Hợp nhất các con của otherChild vào child
                break;                                            // Không cần kiểm tra nữa
            }
        }

        // Nếu không tìm thấy itemset, tạo nút mới
        if (!found)
        {
            NodePPC *newChild = new NodePPC(otherTree->children[i]->itemset);
            newChild->quantity = otherTree->children[i]->quantity; // Cập nhật quantity cho nút mới
            this->children.push_back(newChild);                    // Thêm nút mới vào children
            // Nếu otherChild có con, ta cũng gọi merge để thêm chúng
            if (!otherTree->children[i]->children.empty())
            {
                newChild->merge(otherTree->children[i]);
            }
        }
    }
} 

void NodePPC::append(NodePPC *otherTree)
{
    this->children.push_back(otherTree);
} 