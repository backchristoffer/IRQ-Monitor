CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread

SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = irqmon

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
