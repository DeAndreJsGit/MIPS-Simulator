CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

TARGET = mips_sim

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

debug: CXXFLAGS += -g
debug: clean $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
	