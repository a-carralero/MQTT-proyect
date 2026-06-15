APP     := libmymosquitto.a
CXX     := g++
CC      := gcc
SRC_DIR := src
BUILD_DIR := build

CFLAGS:= -Wall -Wextra -pedantic
ifdef RELEASE
	CFLAGS += -O3
else
	CFLAGS += -g
endif
CXXFLAGS  := $(CFLAGS) -std=c++20

SRCS := $(sort $(shell find $(SRC_DIR) -type f \( -name "*.cpp" -o -name "*.c" \)))
OBJS := $(patsubst $(SRC_DIR)%.cpp,$(BUILD_DIR)%.o,$(SRCS))
OBJS := $(patsubst $(SRC_DIR)%.c,$(BUILD_DIR)%.o,$(OBJS))

include ../../colors.mk

$(APP) : $(OBJS)
	$(call print_blue,Making Static Library: $(APP))
	ar -crs -o $(APP) $(OBJS)
	ranlib $(APP)

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(call print_green,Compiling $<)
	$(CXX) -c $< $(CXXFLAGS) -o $@

$(BUILD_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(call print_green,Compiling $<)
	$(CC) -c $< $(CFLAGS) -o $@ 

info:
	$(info $(SRCS))
	$(info $(OBJS))
	
clean:
	rm -f $(APP)
	rm -rf $(BUILD_DIR)


.PHONY: info clean