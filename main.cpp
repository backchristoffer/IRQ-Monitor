#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

using namespace std;
using namespace chrono;

void parseInterrupts() {
    ifstream file("/proc/interrupts");
    if (!file.is_open()) {
        cerr << "Error: Could not open /proc/interrupts\n";
        return;
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string cpu;
        ss >> cpu;
        cout << "IRQ " << cpu << ":";

        int interruptCount;
        while (ss >> interruptCount) {
            microseconds microseconds_duration = duration_cast<microseconds>(milliseconds(interruptCount));
            cout << " " << microseconds_duration.count();
        }
        cout << endl;
    }

    file.close();
}

int main() {
    while (true) {
        parseInterrupts();
    }

    return 0;
}
