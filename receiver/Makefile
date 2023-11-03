# Specify the compiler
CXX = g++

# Specify the compiler flags
CXXFLAGS = -std=c++17 -pthread

# Target file name
TARGET = improved_recv.o  # This should be the name of the executable file

# Source file name
SRC = improved_recv.cpp

# Rule to build the target file from the source file
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Phony target to clean up the build directory
.PHONY: clean

clean:
	rm -f $(TARGET)

