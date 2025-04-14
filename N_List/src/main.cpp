#include <iostream>
#include <NodePPC.hpp>
#include <PrePosCount.hpp>
#include <chrono>
#include <list>
#include <string>
#include <PPCTree.hpp>
#include <NodeN_List.hpp>
#include <fstream>
#include <filesystem>
#include <thread>
#include <atomic>
#include <conio.h>
#define DATAPATH "data/Dataset/hui/"
#define DATARAW "data/"

namespace fs = std::filesystem;
using namespace std;

atomic<bool> stopProgram(false);

void checkForQuit()
{
    while (!stopProgram)
    {
        if (_kbhit())
        {
            char ch = _getch();
            if (ch == 'q' || ch == 'Q')
            {
                stopProgram = true;
                cout << "\nPhím 'q' được nhấn, đang thoát chương trình...\n";
                break;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "No file provided" << std::endl;
        return 1;
    }

    thread quitThread(checkForQuit);

    std::string filename = argv[1];
    size_t num = 5;
    string inputName = filename;
    ifstream inputFile(DATARAW + inputName + ".txt");
    string label;
    vector<double> values;
    double value;
    if (!inputFile)
    {
        cerr << "Không thể mở tệp." << endl;
        quitThread.detach();
        return 1;
    }

    inputFile >> label;
    cout << "Đã đọc: " << label << endl;

    while (inputFile >> value)
    {
        values.push_back(value);
    }

    inputFile.close();
    string dir = "output";
    if (!fs::exists(dir))
        fs::create_directory(dir);

    ofstream logFile("output/" + label + ".txt", ios::app);
    if (!logFile)
    {
        cerr << "Không thể mở tệp log." << endl;
        quitThread.detach();
        return 1;
    }

    logFile << "Name data: " << label << endl;

    try
    {
        for (double v : values)
        {
            if (stopProgram)
            {
                break;
            }

            string input = label;
            double minws = v;
            logFile << "Minsub: " << v << endl;

            string outputHui = input + ".hui";
            PrePosCount *ppc = new PrePosCount(minws);

            auto startRead = chrono::high_resolution_clock::now();
            // ppc->readDataById(DATAPATH + outputHui);
            ppc->readDataById(DATAPATH + outputHui, num);
            auto endRead = chrono::high_resolution_clock::now();
            auto durationRead = chrono::duration_cast<chrono::microseconds>(endRead - startRead).count();
            logFile << "Time taken for readDataById: " << durationRead / 1e6 << " seconds" << endl;

            auto startTime = chrono::high_resolution_clock::now(); // Bắt đầu đo N_LIST
            auto startSort = chrono::high_resolution_clock::now();
            // Giả sử có hàm sortByCountItem, nếu không thì bỏ phần này
            // ppc->sortByCountItem();
            auto endSort = chrono::high_resolution_clock::now();
            auto durationSort = chrono::duration_cast<chrono::microseconds>(endSort - startSort).count();
            logFile << "Time taken for sortByCountItem: " << durationSort / 1e6 << " seconds" << endl;

            auto startBuild = chrono::high_resolution_clock::now();
            // ppc->buildSingle();
            ppc->buildMulti(num);
            auto endBuild = chrono::high_resolution_clock::now();
            auto durationBuild = chrono::duration_cast<chrono::microseconds>(endBuild - startBuild).count();
            logFile << "Time taken for build: " << durationBuild / 1e6 << " seconds" << endl;

            auto startMarker = chrono::high_resolution_clock::now();
            ppc->ppcMarker(ppc->root, 0);
            auto endMarker = chrono::high_resolution_clock::now();
            auto durationMarker = chrono::duration_cast<chrono::microseconds>(endMarker - startMarker).count();
            logFile << "Time taken for ppcMarker: " << durationMarker / 1e6 << " seconds" << endl;

            PPCTree *ppcTree = new PPCTree(ppc->minsub);
            auto startInitRoot = chrono::high_resolution_clock::now();
            ppcTree->initRoot(ppc->root);
            auto endInitRoot = chrono::high_resolution_clock::now();
            auto durationInitRoot = chrono::duration_cast<chrono::microseconds>(endInitRoot - startInitRoot).count();
            logFile << "Time taken for InitRoot " << durationInitRoot / 1e6 << " seconds" << endl;
            auto startTreeBuild = chrono::high_resolution_clock::now();
            // ppcTree->buildTreeSingle(ppcTree->root);
            // ppcTree->buildTreeMulti(ppcTree->root, num);
            ppcTree->buildTreeMultiFuture(ppcTree->root, num);

            auto endTreeBuild = chrono::high_resolution_clock::now();
            auto durationTreeBuild = chrono::duration_cast<chrono::microseconds>(endTreeBuild - startTreeBuild).count();
            logFile << "Time taken for buildTree: " << durationTreeBuild / 1e6 << " seconds" << endl;

            auto endTime = chrono::high_resolution_clock::now();
            auto durationTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count();
            logFile << "Time taken N_LIST: " << durationTime / 1e6 << " seconds" << endl;
            logFile << "Count Total: " << ppcTree->countTotal << endl;

            delete ppc;
            delete ppcTree;
        }
    }
    catch (const exception &e)
    {
        cerr << "Chương trình gặp lỗi: " << e.what() << endl;
        stopProgram = true;
    }

    logFile << "END" << endl;
    logFile.close();

    stopProgram = true;
    if (quitThread.joinable())
    {
        quitThread.join();
    }

    return 0;
}