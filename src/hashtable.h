#ifndef HASH_H
#define HASH_H

typedef struct HashTable HashTable;

HashTable *htCreate() ;
int htAdd(HashTable *ht, char *key, char *value) ;
char *htLookup(HashTable *ht, char *key) ;
void htDestroy(HashTable *ht);

#endif /* #HASH_H */
