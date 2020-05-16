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

/*
* Create generic hash table and return pointer
* Size: number of elements in hash table. Less -> more collisions
* compareKey: bool fn (const void *a, const void *b)
*       Used to compare keys when looking up hashtable and updating value. Must return true if keys are equal, false if not
* hash: unsigned int fn (const void *key, unsigned int size)   
*       Hash function. Returns index of item in hash table array. More optimised for inputs -> less collisions.
*       Return value must be 0 <= x < size
* copyKey: void *fn (const void *key)
*       Used to copy key provided to htAdd(), for storage in table. Returns a new copy of key.
* copyValue: void *fn (const void *value)
*       Used to copy value provided to htAdd(), for storage in table. Returns a new copy of value.
* freeKey: void fn (void *key)
*       Used to free object returned from copyKey, when hash table is destroyed
* freeValue: void fn (void *value)
*       Used to free object returned from copyValue, when hash table is destroyed
*/
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

/*
* Add element to hashtable as ht[key] = value
* return 0 for success and 1 for failure
*/
int htAdd(HashTable *ht, const void *key, const void *value) 
{
    unsigned int keyhash = ht->hash(key, ht->size);
    ht->table[keyhash] = elistAdd(ht->table[keyhash], ht->compare, ht->copyKey, ht->copyValue, key, value);
    if (ht->table[keyhash] == NULL) { return 1; }
    return 0;
}

/*
* Returns pointer to value for given key
* Returns NULL on key not found
*/
void *htLookup(HashTable *ht, const void *key) 
{
    if (ht == NULL || ht->table == NULL) { return NULL; }
    unsigned int keyhash = ht->hash(key, ht->size);
    char *value = elistFind(ht->table[keyhash], ht->compare, key);
    return value;
}

/*
* Destroy (free) hashtable
*/
void htDestroy(HashTable *ht)
{
    for (unsigned int i = 0; i < ht->size; i++) {
        freeList(ht->table[i], ht->freeKey, ht->freeValue);
    }
    free(ht->table);
    free(ht);
}

/*
* Add item to elist , copying key and value into list.
* Uses compare to compare keys.
* Returns new beginning of list
*/
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

/*
* Return item in elist l whose key matches key, or NULL if no such item
*/
void *elistFind(elist *l, compareFn compare, const void *key)
{
    for (elist *cur = l; cur != NULL; cur = cur->next) {
        if (compare(cur->key, key)) {
            return cur->value;
        }
    }
    return NULL;
}

/*
* Free elist l
*/
void freeList(elist *l, freeFn freeKey, freeFn freeValue)
{
    if (l != NULL) {
        freeList(l->next, freeKey, freeValue);
        freeKey(l->key);
        freeValue(l->value);
        free(l);
    }
}
