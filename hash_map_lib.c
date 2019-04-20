#include <stdio.h>
#include "hash_map_lib.h"

map_key_t clone_key(map_key_t key) {
    map_key_t clone = malloc(strlen(key));
    if (clone == NULL) return NULL;
    strcpy(clone, key);
    return clone;
}


map_t new_hash_map() {
    return new_hash_map_of_size(HASH_MAP_SLOTS_DEFAULT);
}


map_t new_hash_map_of_size(unsigned int slots_len) {
    struct hash_map *map = malloc(sizeof(struct hash_map) + sizeof(struct hash_map_entry) * slots_len);
    if (map == NULL) return NULL;

    map->entries = 0;
    map->slots_len = slots_len;
    for (unsigned int i = 0; i < slots_len; i++) map->slots[i] = NULL;

    return map;
}


void hash_map_free(map_t map) {
    for (unsigned int i = 0; i < map->slots_len; i++) {
        struct hash_map_entry *entry = map->slots[i];
        while (entry) {
            struct hash_map_entry *next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(map);
}


map_t hash_map_grow(map_t map) {
    map_t new_map = new_hash_map_of_size(map->slots_len * 2);
    if (new_map == NULL) return NULL;

    for (unsigned int i = 0; i < map->slots_len; i++) {
        struct hash_map_entry *entry = map->slots[i];
        while (entry) {
            if (hash_map_set(new_map, entry->key, entry->value) == NULL) return NULL;
            entry = entry->next;
        }
    }

    hash_map_free(map);
    return new_map;
}


map_t hash_map_set(map_t map, map_key_t key, map_value_t value) {
    key = clone_key(key);
    if (key == NULL) return NULL;

    if (map->entries == map->slots_len && (map = hash_map_grow(map)) == NULL) return NULL;

    unsigned int i = hash_code(key) % map->slots_len;

    struct hash_map_entry **entry = &map->slots[i];
    while (*entry && strcmp((*entry)->key, key)) entry = &(*entry)->next;

    if (*entry == NULL) {
        *entry = malloc(sizeof (struct hash_map_entry));
        if (*entry == NULL) return NULL;

        (*entry)->next = NULL;
        (*entry)->key = key;
        map->entries++;
    }

    (*entry)->value = value;
    
    return map;
}


map_value_t hash_map_get(map_t map, map_key_t key) {
    unsigned int i = hash_code(key) % map->slots_len;

    struct hash_map_entry *entry = map->slots[i];
    while (entry && strcmp(entry->key, key)) entry = entry->next;

    return entry ? entry->value : NULL;
}


map_value_t hash_map_del(map_t map, map_key_t key) {
    unsigned int i = hash_code(key) % map->slots_len;
    map_value_t value = NULL;

    struct hash_map_entry **prev = &map->slots[i];
    struct hash_map_entry *entry = map->slots[i];
    while (entry && strcmp(entry->key, key)) {
        prev = &(*prev)->next;
        entry = entry->next;
    }
    
    if (entry) {
        value = entry->value;
        *prev = entry->next;
        map->entries--;
        free(entry->key);
        free(entry);
    }

    return value;
}

void hash_map_print_value(map_value_t value) {
    printf("%p", value);
}

void hash_map_print(map_t map, int pretty, void (*value_fmt)(map_value_t)) {
    if (value_fmt == NULL) {
        value_fmt = &hash_map_print_value;
    }
    if (pretty) {
        printf("{\n");
        unsigned int entry_c = 0;
        for (unsigned int i = 0; i < map->slots_len; i++) {
            struct hash_map_entry *entry = map->slots[i];
            while (entry) {
                printf("    \"%s\": ", entry->key);
                (*value_fmt)(entry->value);
                if (entry_c + 1 < map->entries) {
                    printf(",");
                }
                printf("\n");
                entry = entry->next;
                entry_c++;
            }
        }
        printf("}\n");
    } else {
        for (unsigned int i = 0; i < map->slots_len; i++) {
            printf("[%d] -> ", i);
            struct hash_map_entry *entry = map->slots[i];
            while (entry) {
                printf("\"%s\": ", entry->key);
                (*value_fmt)(entry->value);
                printf(" -> ");
                entry = entry->next;
            }
            printf("%s\n", NULL);
        }
    }
}

unsigned long djb2(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = hash * 33 + c;
    
    return hash;
}


unsigned long hash_code(map_key_t key) {
    return djb2(key);
}
