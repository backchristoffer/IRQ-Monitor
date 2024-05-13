#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sched.h>

std::unordered_map<std::string, std::vector<std::string>> parse_interrupts() {
    std::ifstream file("/proc/interrupts");
    std::unordered_map<std::string, std::vector<std::string>> interrupts;

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find(':') != std::string::npos) {
                std::istringstream iss(line);
                std::string irq;
                iss >> irq;

                std::string devices;
                while (iss >> devices) {
                    interrupts[irq].push_back(devices);
                }
            }
        }
        file.close();
    }
    return interrupts;
}

void print_interrupts(const std::unordered_map<std::string, std::vector<std::string>>& interrupts) {
    std::cout << "IRQ\tDevice" << std::endl;
    for (const auto& entry : interrupts) {
        std::cout << entry.first << "\t";
        for (const auto& device : entry.second) {
            std::cout << device << " ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    int cpu_affinity = 0;

    int opt;
    while ((opt = getopt(argc, argv, "c:")) != -1) {
        switch (opt) {
            case 'c':
                cpu_affinity = std::atoi(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-c CPU_AFFINITY]" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_affinity, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }

    while (true) {
        auto interrupts = parse_interrupts();
        print_interrupts(interrupts);
        usleep(100000); 
    }
    return 0;
}