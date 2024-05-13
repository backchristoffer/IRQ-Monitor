# Playing with irq monitoring in real-time (ms)
## How to run the program on CPU1 but still monitoring all available CPUs
```
./irqmon --cpu-affinity 1
```
g++ -o irqmon main.cpp -lpthread -std=c++11 -Wall
