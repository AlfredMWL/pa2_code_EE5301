CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic

TARGET := fastplace
SOURCES := main.cpp placement.cpp sparse_matrix.cpp system_builder.cpp \
           matrix_assembler.cpp net_model.cpp io_writer.cpp debug.cpp \
           suraj_parser.cpp
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all clean plot

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

plot:
	@test -n "$(BASE)" || (echo "Usage: make plot BASE=toy01"; exit 2)
	python3 plot_placement.py "$(BASE)"

clean:
	rm -f $(TARGET) $(OBJECTS)