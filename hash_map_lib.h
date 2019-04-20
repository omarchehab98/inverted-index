#include <stdlib.h>
#include <string.h>

#define HASH_MAP_SLOTS_DEFAULT 4

typedef struct hash_map *map_t;
typedef char *map_key_t;
typedef void *map_value_t;

struct hash_map {
    unsigned int entries;
    unsigned int slots_len;
    struct hash_map_entry *slots[];
};

struct hash_map_entry {
    struct hash_map_entry *next;
    map_key_t key;
    map_value_t value;
};

map_t new_hash_map();

map_t new_hash_map_of_size(unsigned int size);

void hash_map_free(map_t map);

map_t hash_map_set(map_t map, map_key_t key, map_value_t value);

map_value_t hash_map_get(map_t map, map_key_t key);

map_value_t hash_map_del(map_t map, map_key_t key);

void hash_map_print(map_t map, int pretty, void (*value_fmt)(map_value_t));

unsigned long hash_code(map_key_t key);
