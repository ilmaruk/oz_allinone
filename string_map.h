#ifndef _STRING_MAP_H
#define _STRING_MAP_H

#include <stddef.h>
#include <string.h>

#define MAP_MAX_ENTRIES 10

typedef struct {
    const char *key;
    const char *value;
} map_entry_t;

typedef struct {
    map_entry_t entries[MAP_MAX_ENTRIES];
    size_t count;
} string_map_t;

void map_init(string_map_t *map) {
    map->count = 0;
}

int map_set(string_map_t *map, const char *key, const char *value) {
    if (!map || !key || !value) return 0;

    for (size_t i = 0; i < map->count; i++) {
        if (strcmp(map->entries[i].key, key) == 0) {
            map->entries[i].value = value;
            return 1;
        }
    }

    if (map->count >= MAP_MAX_ENTRIES) return 0;

    map->entries[map->count++] = (map_entry_t) {
        .key = key,
        .value = value
    };

    return 1;
}

const char *map_get(const string_map_t *map, const char *key) {
    if (!map || !key) return NULL;

    for (size_t i = 0; i < map->count; i++) {
        if (strcmp(map->entries[i].key, key) == 0) {
            return map->entries[i].value;
        }
    }

    return NULL;
}

int map_contains(const string_map_t *map, const char *key) {
    return map_get(map, key) != NULL;
}

void map_clear(string_map_t *map) {
    if (map) map->count = 0;
}

#endif // _STRING_MAP