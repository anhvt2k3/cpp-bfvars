CC=g++

.PHONY: clean

all: data_gen main

data_gen: data/data_gen.cpp
	$(CC) -o ./data/data_gen.exe ./data/data_gen.cpp

main: main.cpp
	$(CC) -std=c++17 -o ./main.exe \
		./utils/hash/rurban-smhasher/MurmurHash3.cpp \
		./utils/hash/rurban-smhasher/xxhash.c \
		./main.cpp \
		-l ssl -l crypto

clean:
	rm -f ./data/data_gen.exe ./main.exe
 
