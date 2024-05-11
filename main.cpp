#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <pthread.h>
#include <unistd.h>  

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
        stringstream ss(line);
        string cpuStr;
        ss >> cpuStr;
        if (cpuStr.find("CPU") == 0) {
            int cpu = stoi(cpuStr.substr(3)); 
            if (find(monitoredCPUs.begin(), monitoredCPUs.end(), cpu) != monitoredCPUs.end()) {
                vector<int> counts;
                int count;
                while (ss >> count) {
                    counts.push_back(count);
                }
                interruptCounts.push_back(counts);
            }
        }
    }

    procInterrupts.close();

    return interruptCounts;
}

void monitorInterrupts(const vector<int>& monitoredCPUs, int programCPU, const int durationSeconds, const string& logFilePath) {
    setCPUAffinity(programCPU);

    ofstream logfile(logFilePath);
    if (!logfile.is_open()) {
        cerr << "Error: Failed to open logfile for writing" << endl;
        return;
    }

    logfile << "Program started. Monitoring interrupts on CPUs: ";
    for (int cpu : monitoredCPUs) {
        logfile << cpu << " ";
    }
    logfile << ". Program running on CPU: " << programCPU << endl;


    auto startTime = chrono::steady_clock::now();
    auto endTime = chrono::steady_clock::now() + chrono::seconds(durationSeconds);
    while (chrono::steady_clock::now() < endTime || durationSeconds == -1) {
        vector<vector<int>> interruptCounts = getInterruptCounts(monitoredCPUs);

        logfile << "Interrupt activity:";
        for (size_t i = 0; i < monitoredCPUs.size(); ++i) {
            logfile << " CPU" << monitoredCPUs[i] << ": ";
            for (int count : interruptCounts[i]) {
                logfile << count << " ";
            }
            logfile << " |";
        }
        logfile << endl;

        sleep(1);
    }

    logfile << "Program ended." << endl;

    logfile.close();
}

int main(int argc, char* argv[]) {
    vector<int> monitoredCPUs;
    int programCPU = -1;
    int durationSeconds = -1;
    string logFilePath = "interrupt_monitor.log";

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--cpu-monitoring") == 0) {
            i++;
            while (i < argc && isdigit(*argv[i])) {
                monitoredCPUs.push_back(atoi(argv[i]));
                i++;
            }
        } else if (strcmp(argv[i], "--cpu-run") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                programCPU = atoi(argv[i]);
            }
        } else if (strcmp(argv[i], "--seconds") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                durationSeconds = atoi(argv[i]);
            }
        }
    }

    if (monitoredCPUs.empty() || programCPU == -1) {
        cerr << "Error: Required command-line arguments not provided" << endl;
        return 1;
    }

    monitorInterrupts(monitoredCPUs, programCPU, durationSeconds, logFilePath);

    return 0;
}
