#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <pthread.h>
#include <unistd.h>  
#include <limits>
#include <ios>
using namespace std;

void setCPUAffinity(int cpu) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        cerr << "Error: Failed to set CPU affinity" << endl;
    }
}

vector<vector<int>> getInterruptCounts(const vector<int>& monitoredCPUs) {
    vector<vector<int>> interruptCounts;

    ifstream procInterrupts("/proc/interrupts");
    if (!procInterrupts.is_open()) {
        cerr << "Error: Failed to open /proc/interrupts" << endl;
        return interruptCounts;
    }

    string line;
    while (getline(procInterrupts, line)) {
        cout << "Read line: " << line << endl;
        stringstream ss(line);
        string cpuStr;
        ss >> cpuStr;
        if (cpuStr.find("CPU") == 0) {
            int cpu = stoi(cpuStr.substr(3));
            if (find(monitoredCPUs.begin(), monitoredCPUs.end(), cpu) != monitoredCPUs.end()) {
                vector<int> counts;
                int count;
                ss.ignore(numeric_limits<streamsize>::max(), ':');
                while (ss >> count) {
                    counts.push_back(count);
                }
                interruptCounts.push_back(counts);
                cout << "Pushed interrupt counts for CPU" << cpu << " with size: " << counts.size() << endl; // Debug statement
            }
        }
    }

    procInterrupts.close();

    return interruptCounts;
}


void monitorInterrupts(const vector<int>& monitoredCPUs, int programCPU) {
    setCPUAffinity(programCPU);

    while (true) {
        vector<vector<int>> interruptCounts = getInterruptCounts(monitoredCPUs);

        cout << "Size of interruptCounts: " << interruptCounts.size() << endl; 

        if (!interruptCounts.empty()) {
            cout << "Interrupt activity:" << endl;
            for (size_t i = 0; i < monitoredCPUs.size(); ++i) {
                cout << " CPU" << monitoredCPUs[i] << ": ";
                cout << "Size of inner vector: " << interruptCounts[i].size() << endl;
                for (int count : interruptCounts[i]) {
                    cout << count << " ";
                }
                cout << " |";
            }
            cout << endl;
        } else {
            cerr << "Error: No interrupt counts retrieved" << endl;
        }

        cout << "End of iteration" << endl;

        sleep(1);
    }
}


int main() {
    vector<int> monitoredCPUs = {0, 1};
    int programCPU = 2;

    cout << "Monitored CPUs: ";
    for (int cpu : monitoredCPUs) {
        cout << cpu << " ";
    }
    cout << endl;
    cout << "Program CPU: " << programCPU << endl;

    monitorInterrupts(monitoredCPUs, programCPU);

    return 0;
}
