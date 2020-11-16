SRC_DIR = ./src
TEST_DIR = ./test
OBJ = md5.o test.o
EXE = md5_test
CC = gcc

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE)

$(OBJ): $(SRC_DIR)/md5.h

md5.o:
	$(CC) -c $(SRC_DIR)/md5.c

test.o:
	$(CC) -c $(TEST_DIR)/test.c

clean:
	rm -f $(OBJ) $(EXE)