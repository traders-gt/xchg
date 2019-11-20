CXX := g++
CXXFLAGS := -std=c++2a -O3 -Wall -Wpedantic -MMD
-include $(OBJ_FILES:.o=.d)

SOURCES := $(wildcard src/*.cc)
OBJECTS := $(patsubst src/%.cc, build/%.o, $(SOURCES))
DEPENDENCY_FILES := $(patsubst src/%.cc, build/%.d, $(SOURCES))
TARGET := xchg

.PHONY: clean

all: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPENDENCY_FILES)


$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

