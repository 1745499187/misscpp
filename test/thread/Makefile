SHELL = /bin/sh

CXX = gcc
LD = gcc
CXXFLAGS = -g -Wall
LDFLAGS = -g

TGT_DIR = .
SRC_DIR = .
OBJ_DIR = .

TGT_FILE = tester
TARGET = $(TGT_DIR)/$(TGT_FILE)

INCLS = -I$(SRC_DIR) -I../..
LIB_PATH = -L$(OBJ_DIR)
LIBS = $(LIB_PATH) -lstdc++ -llog4cxx -lpthread

SRC_LIST = $(wildcard $(SRC_DIR)/*.cc)

OBJS = $(foreach item, $(SRC_LIST), $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(basename $(notdir $(item))))))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	$(CXX) -c $(CXXFLAGS) $(INCLS) -o $@ $<

clean:
	rm -rf $(TARGET) $(OBJS)

test:
	@echo $(OBJS)
	