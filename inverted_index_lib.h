#include <stdlib.h>
#include <math.h>
#include "hash_map_lib.h"

struct inverted_index_occurrences {
    unsigned int refs_len;
    unsigned int refs[];
};

map_t inverted_index(char *document);
