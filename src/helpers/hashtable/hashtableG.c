#include <stdlib.h>
#include <string.h>

#include "hashtableG.h"
//#include "constants.h"

typedef struct elemlist elist;
struct elemlist {
    void *key;
    void *value;
    elist *next;
};

struct HashTable {
    elist **table;
    unsigned int size;
    compareFn compare;
    hashFn hash;
    copyFn copyKey;
    copyFn copyValue;
    freeFn freeKey;
    freeFn freeValue;
};

elist *elistAdd(elist *l, compareFn compare, copyFn copyKey, copyFn copyValue, const void *key, const void *value);
void *elistFind(elist *l, compareFn compare, const void *key);
void freeList(elist *l, freeFn freeKey, freeFn freeValue);

HashTable *htCreate(unsigned int size, compareFn compareKey, hashFn hash, 
    copyFn copyKey, copyFn copyValue, freeFn freeKey, freeFn freeValue)
{
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        return NULL;
    }
    ht->size = size;
    ht->compare = compareKey;
    ht->hash = hash;
    ht->copyKey = copyKey;
    ht->copyValue = copyValue;
    ht->freeKey = freeKey;
    ht->freeValue = freeValue;
    ht->table = malloc(ht->size * sizeof(*ht->table)); // sizeof(elist *)
    if (ht->table == NULL) {
        free(ht);
        return NULL;
    }
    memset(ht->table, 0, ht->size * sizeof(*ht->table));
    return ht;
}

// return 0 for success and 1 for failure
int htAdd(HashTable *ht, const void *key, const void *value) 
{
    unsigned int keyhash = ht->hash(key, ht->size);
    ht->table[keyhash] = elistAdd(ht->table[keyhash], ht->compare, ht->copyKey, ht->copyValue, key, value);
    if (ht->table[keyhash] == NULL) { return 1; }
    return 0;
}

// return pointer for success and NULL on failure
void *htLookup(HashTable *ht, const void *key) 
{
    if (ht == NULL || ht->table == NULL) { return NULL; }
    unsigned int keyhash = ht->hash(key, ht->size);
    char *value = elistFind(ht->table[keyhash], ht->compare, key);
    return value;
}

void htDestroy(HashTable *ht)
{
    for (unsigned int i = 0; i < ht->size; i++) {
        freeList(ht->table[i], ht->freeKey, ht->freeValue);
    }
    free(ht->table);
    free(ht);
}

// return new beginning of list
elist *elistAdd(elist *l, compareFn compare, copyFn copyKey, copyFn copyValue, const void *key, const void *value)
{
    for (elist *cur = l; cur != NULL; cur = cur->next) {
        if (compare(cur->key, key)) {
            cur->value = copyValue(value);
            return l;
        }
    }
    // either list is empty and l is NULL, or list is not empty
    // either way, we set cur->next to l

    elist *cur = malloc(sizeof(elist));
    if (cur == NULL) {
        return NULL;
    }
    cur->key = copyKey(key);
    cur->value = copyValue(value);
    cur->next = l;
    return cur;
}

void *elistFind(elist *l, compareFn compare, const void *key)
{
    for (elist *cur = l; cur != NULL; cur = cur->next) {
        if (compare(cur->key, key)) {
            return cur->value;
        }
    }
    return NULL;
}

void freeList(elist *l, freeFn freeKey, freeFn freeValue)
{
    if (l != NULL) {
        freeList(l->next, freeKey, freeValue);
        freeKey(l->key);
        freeValue(l->value);
        free(l);
    }
}
