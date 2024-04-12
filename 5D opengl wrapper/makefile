CC = gcc
CXX = g++
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Isrc/headers -Idependencies/include
CXXFLAGS = -std=c++11 -Wall -Wextra -Wpedantic -Isrc/headers -Idependencies/include
LDFLAGS = -Ldependencies/lib -lopengl32 -lglfw3 -lglew32 -lgdi32

# Directory for object files
OBJ_DIR = obj

# Source files
SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
EXECUTABLE = main

# Rule to create directory if it doesn't exist
$(shell mkdir -p $(OBJ_DIR))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

# Rule to compile .c files into object files in the specified directory
$(OBJ_DIR)/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)