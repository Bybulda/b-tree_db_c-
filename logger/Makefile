CC=g++
CFLAGS=
LDFLAGS=-lm -std=c++17
SOURCES=logger_builder.cpp logger_builder_concrete.cpp logger.cpp logger_concrete.cpp main.cpp
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

.PRECIOUS: %.o