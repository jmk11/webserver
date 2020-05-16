#ifndef HASHTABLEG_H
#define HASHTABLEG_H

#include "../bool/bool.h"

typedef struct HashTable HashTable;

/*
struct HashTable {
    HashTableI *table;
    int (*add)(const void *key, const void *value);
    void* (*value)(const void *key);
    void (*destroy)(void);
};
// You can't not pass the hashtable itself becuase you have no other way to get it
*/

typedef bool (*compareFn) (const void *, const void *);
typedef unsigned int (*hashFn) (const void *, unsigned int);
typedef void *(*copyFn) (const void *);
typedef void (*freeFn) (void *);

HashTable *htCreate(unsigned int size, compareFn compare, hashFn hash, 
    copyFn copyKey, copyFn copyValue, freeFn freeKey, freeFn freeValue);
int htAdd(HashTable *ht, const void *key, const void *value) ;
void *htLookup(HashTable *ht, const void *key);
void htDestroy(HashTable *ht);

#endif /* #HASHTABLEG_H */
