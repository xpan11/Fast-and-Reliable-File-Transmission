# This is the name of the output file
OUTPUT = improved_sender.o

# These are the flags for the g++ compiler
CXXFLAGS = -std=c++11 -pthread

# This is the name of the source file
SRC = improved_sender.cpp

# This is the rule for creating the output file from the source file
$(OUTPUT): $(SRC)
	g++ $(CXXFLAGS) $(SRC) -o $(OUTPUT)

# This is a rule for cleaning up the output files
clean:
	rm -f $(OUTPUT)

