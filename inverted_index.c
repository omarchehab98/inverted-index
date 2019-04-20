#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "inverted_index_lib.h"

char *usage = "usage: %s <file>\n";

void hash_map_print_value_occurrences(map_value_t value) {
    struct inverted_index_occurrences *occurrences = value;
    printf("[");
    for (int i = 0; i < occurrences->refs_len; i++) {
        printf("%d", occurrences->refs[i]);
        if (i + 1 < occurrences->refs_len) {
            printf(", ");
        }
    }
    printf("]");
}

int main(int argc, char **argv) {
    if (argc != 2) printf(usage, argv[0]), exit(1);

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) perror(argv[0]), exit(1);
    
    char *document;
    unsigned long document_len;
    
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) perror(argv[0]), exit(1);
    document_len = (unsigned long) file_stat.st_size;

    document = (char *) mmap(0, document_len, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0);
    if (document == NULL) perror(argv[0]), exit(1);
    
    map_t map = inverted_index(document);
    hash_map_print(map, 1, &hash_map_print_value_occurrences);

    return 0;
}
