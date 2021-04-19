# Usual compilation flags
CC = gcc
CFLAGS = -c

# Resources
INC = $(wildcard ./src/*.h)
SRC = $(wildcard ./src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

# target
TARGET = md5

all: $(TARGET) 

$(TARGET): $(OBJ) $(INC)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)