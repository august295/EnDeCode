SRC_DIR = ./src
TEST_DIR = ./test
OBJ = file_operat.o des.o test.o
EXE = des_test
CC = gcc

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE)

$(OBJ): $(SRC_DIR)/*.h

test.o:
	$(CC) -c $(TEST_DIR)/test.c

des.o:
	$(CC) -c $(SRC_DIR)/des.c

file_operat.o:
	$(CC) -c $(SRC_DIR)/file_operat.c

clean:
	rm -f $(OBJ) $(EXE)