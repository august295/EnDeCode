SRC_DIR = ./src
TEST_DIR = ./test
OBJ =  des.o test.o
EXE = des_test
CC = gcc

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE)

$(OBJ): $(SRC_DIR)/*.h

test.o:
	$(CC) -c $(TEST_DIR)/test.c

des.o:
	$(CC) -c $(SRC_DIR)/des.c

clean:
	rm -f $(OBJ) $(EXE)