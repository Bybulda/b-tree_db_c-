# CC=g++
# CFLAGS=
# LDFLAGS=-lm -std=c++2a
# SOURCES=../logger/logger_builder.cpp ../logger/logger_builder_concrete.cpp ../logger/logger.cpp ../logger/logger_concrete.cpp memory.cpp memory_concrete.cpp main.cpp
# OBJECTS=$(SOURCES:.c=.o)
# EXECUTABLE=main

# all: $(SOURCES) $(EXECUTABLE)

# $(EXECUTABLE): $(OBJECTS)
# 	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE)

# # clean:
# # 	rm -f $(EXECUTABLE)
# CXX = GCC
# CXXFLAGS = -Wall -Wextra -pedantic -std=c++20
# SRC = ../logger/logger_builder.cpp ../logger/logger_builder_concrete.cpp ../logger/logger.cpp ../logger/logger_concrete.cpp memory.cpp memory_concrete.cpp main.cpp
# OBJ = $(SRC:.cpp=.o)
# EXE = main


# all: $(EXE)


# $(EXE): $(OBJ)
# 	$(CXX) $(CXXFLAGS) -o $@ $^

# clean:
# 	rm -f $(OBJ) $(EXE)
CC=g++
CFLAGS=
LDFLAGS=-lm -std=c++20
SOURCES=../logger/logger_builder.cpp ../logger/logger_builder_concrete.cpp ../logger/logger.cpp ../logger/logger_concrete.cpp memory.cpp memory_concrete.cpp main.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE)

clean:
	rm -f $(OBJ) $(EXECUTABLE)

.PRECIOUS: %.o