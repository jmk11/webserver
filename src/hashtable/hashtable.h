#ifndef HASHTABLE_H
#define HASHTABLE_H

typedef struct HashTable HashTable;

HashTable *htCreate() ;
int htAdd(HashTable *ht, const char *key, const char *value) ;
char *htLookup(HashTable *ht, const char *key);
void htDestroy(HashTable *ht);

#endif /* #HASHTABLE_H */
