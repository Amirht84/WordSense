SRC_DIR = Source
BIN_DIR = Bin
HED_DIR = Include

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.o, $(SRCS))

CXX = g++
CFLAGS = -Wall -g -I$(HED_DIR)

TARGET ?= main.out

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $^ -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(OBJS)
	@rMpt
