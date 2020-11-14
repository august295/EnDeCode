SRC_DIR = ./src
TEST_DIR = ./test
OBJ = md5.o test.o
EXE = md5_test
CC = g++


$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(SRC_DIR)/* $(TEST_DIR)/*

md5.o:
	$(CC) -c $(SRC_DIR)/md5.c

test.o:
	$(CC) -c $(TEST_DIR)/test.c

clean:
	rm -f *.o