GCC=gcc -Wall -O3

all : hash_map inverted_index

hash_map : hash_map.o hash_map_lib.o
	$(GCC) -o hash_map $^

inverted_index : inverted_index.o inverted_index_lib.o hash_map_lib.o
	$(GCC) -o inverted_index $^

%.o : %.c
	$(GCC) -c $<

clean:
	rm -f *.o hash_map inverted_index *~
