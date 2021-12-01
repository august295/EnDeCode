CC = gcc

main: main.o crc.o
	$(CC) main.o crc.o -o main

main.o: main.cpp crc.h
	$(CC) -c main.cpp -o main.o

crc.o: crc.cpp crc.h type.h
	$(CC) -c crc.cpp -o crc.o

clean:
	rm main *.o