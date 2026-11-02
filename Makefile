.DEFAULT_GOAL := run

DBG ?= DEBUG

TARGET := triangles
BUILD_DIR := build

SRCS := src/main.cpp src/graphic.cpp src/triangles.cpp
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

OS := $(shell uname -s)

ifeq ($(OS),Darwin)
    CXX := clang++

    BREW_PREFIX := $(shell brew --prefix 2>/dev/null)
    ifneq ($(BREW_PREFIX),)
        CPPFLAGS += -I$(BREW_PREFIX)/include
        LDFLAGS += -L$(BREW_PREFIX)/lib
    endif

    LDLIBS := -lGLEW -lglfw -lgtest_main -lgtest \
              -framework OpenGL -framework Cocoa \
              -framework IOKit -framework QuartzCore
else
    CXX := g++
    LDLIBS := -lGLEW -lglfw -lGL -lgtest_main -lgtest
endif

CPPFLAGS += -Iinclude

CXXFLAGS += -std=c++17 \
            -Wall -Wextra -Wpedantic \
            -Wcast-align -Wcast-qual \
            -Wconversion -Wsign-conversion \
            -Wshadow -Wnon-virtual-dtor \
            -Woverloaded-virtual \
            -Wformat=2 \
            -Wundef \
            -Werror=vla \
            -fno-omit-frame-pointer \
            -pthread \
            -MMD -MP

LDFLAGS += -pthread

ifeq ($(DBG),DEBUG)
    CPPFLAGS += -D_DEBUG
    CXXFLAGS += -O1 -g3
    SANITIZERS := -fsanitize=address,undefined
    CXXFLAGS += $(SANITIZERS)
    LDFLAGS += $(SANITIZERS)
else
    CPPFLAGS += -DNDEBUG
    CXXFLAGS += -O2 -g -flto
    LDFLAGS += -flto
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPS)

.PHONY: all run clean