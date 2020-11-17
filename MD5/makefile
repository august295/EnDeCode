CC = gcc
CFLAGS = -c
TARGET = md5
INC = $(wildcard ./src/*.h)
SRC = $(wildcard ./src/*.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(TARGET) 

$(TARGET): $(OBJ) $(INC)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)