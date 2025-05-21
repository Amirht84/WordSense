include ../Configs/config.mk

SRC_DIR = Source
OBJ_DIR = Bin
HED_DIR = Include

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

CXX = g++
CFLAGS = -Wall -g -I$(HED_DIR)
TARGET = $(TestExe)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET)
