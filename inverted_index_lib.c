#include "inverted_index_lib.h"

#define INVERTED_INDEX_REFS_DEFAULT 4


int is_pow2(double x) {
    double y = log2(x);
    return y == floor(y);
}


struct inverted_index_occurrences *add_ref(map_t *map, char *word, unsigned int ref) {
    struct inverted_index_occurrences *occurrences = hash_map_get(*map, word);

    if (occurrences == NULL) {
        occurrences = malloc(sizeof (struct inverted_index_occurrences) + sizeof(int) * INVERTED_INDEX_REFS_DEFAULT);
        if (occurrences == NULL) return NULL;

        occurrences->refs_len = 0;

        *map = hash_map_set(*map, word, occurrences);
        if (*map == NULL) return NULL;
    }

    occurrences->refs_len++;
    if (occurrences->refs_len >= INVERTED_INDEX_REFS_DEFAULT && is_pow2(occurrences->refs_len)) {
        struct inverted_index_occurrences *new_occurrences = malloc(sizeof (struct inverted_index_occurrences) + sizeof(int) * (occurrences->refs_len * 2));
        if (new_occurrences == NULL) return NULL;

        new_occurrences->refs_len = occurrences->refs_len;
        for (int i = 0; i < occurrences->refs_len - 1; i++) {
            new_occurrences->refs[i] = occurrences->refs[i];
        }

        *map = hash_map_set(*map, word, new_occurrences);
        if (*map == NULL) return NULL;

        occurrences = new_occurrences;
    }

    occurrences->refs[occurrences->refs_len - 1] = ref;

    return occurrences;
}


int word_end(char c) {
    return c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z';
}


void strlowercase(char *word) {
    for (short i = 0; word[i] != '\0'; i++) {
        if (word[i] >= 'A' && word[i] <= 'Z') {
            word[i] += 'a' - 'A';
        }
    }
}


map_t inverted_index(char *document) {
    char *word = malloc(255);
    if (word == NULL) return NULL;

    map_t map = new_hash_map();
    int offset = 0;
    int word_len = 0;
    while (1) {
        char c = document[offset];
        if (word_end(c)) {
            if (word_len > 0) {
                word[word_len] = '\0';

                strlowercase(word);
                if (add_ref(&map, word, offset - word_len) == NULL) return NULL;

                word_len = 0;
            }
        } else {
            word[word_len++] = c;
        }
        offset++;
        if (c == '\0') break;
    }

    return map;
}
