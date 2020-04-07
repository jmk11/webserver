#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "hash.h"
//#include "constants.h"

#define HASHSIZE 200

typedef struct elemlist elist;
struct elemlist {
    char *key;
    char *value;
    elist *next;
};

struct HashTable {
    elist **table;
    unsigned int size;
};

elist *elistAdd(elist *l, const char *key, const char *value);
char *elistFind(elist *l, const char *key);
void freeList(elist *l);

HashTable *htCreate() 
{
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        return NULL;
    }
    ht->size = HASHSIZE;
    ht->table = malloc(ht->size * sizeof(elist *));
    if (ht->table == NULL) {
        free(ht);
        return NULL;
    }
    memset(ht->table, 0, ht->size * sizeof(elist *));
    return ht;
}

int htAdd(HashTable *ht, const char *key, const char *value) 
{
    unsigned int keyhash = hash(key, ht->size);
    ht->table[keyhash] = elistAdd(ht->table[keyhash], key, value);
    if (ht->table[keyhash] == NULL) { return 1;}
    return 0;
}

char *htLookup(HashTable *ht, const char *key) 
{
    if (ht == NULL || ht->table == NULL) { return NULL; }
    unsigned int keyhash = hash(key, ht->size);
    char *value = elistFind(ht->table[keyhash], key);
    return value;
}

void htDestroy(HashTable *ht)
{
    for (unsigned int i = 0; i < HASHSIZE; i++) {
        freeList(ht->table[i]);
    }
    free(ht->table);
    free(ht);
}

// return new beginning of list
elist *elistAdd(elist *l, const char *key, const char *value)
{
    for (elist *cur = l; cur != NULL; cur = cur->next) {
        if (strcmp(cur->key, key) == 0) {
            cur->value = strdup(value);
            return l;
        }
    }
    // either list is empty and l is NULL, or list is not empty
    // either way, we set cur->next to l

    elist *cur = malloc(sizeof(elist));
    if (cur == NULL) {
        return NULL;
    }
    cur->key = strdup(key);
    cur->value = strdup(value);
    cur->next = l;
    return cur;
}

char *elistFind(elist *l, const char *key)
{
    for (elist *cur = l; cur != NULL; cur = cur->next) {
        if (strcmp(cur->key, key) == 0) {
            return cur->value;
        }
    }
    return NULL;
}

void freeList(elist *l)
{
    freeList(l->next);
    free(l->key);
    free(l->value);
    free(l);
}