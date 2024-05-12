# Playing with irq monitoring in real-time (ms)
## How to run
```
./interrupt_monitor --cpu-monitoring 0 1 --cpu-run 2 --seconds 60

```
g++ -o interrupt_monitor interrupt_monitor.cpp -lpthread
