#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

// add new item to table w/ key and val as args
static ht_item* ht_new_item(const char* k, const char* v){
    ht_item* i = malloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}

ht_hash_table* ht_new(){
    ht_hash_table* ht = malloc(sizeof(ht_hash_table));
    ht->size = 53; // fixed size at 53
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(ht_item*)); // alloc NULL bytes to initialize
    return ht;
}

static void ht_del_item(ht_item* i){
    free(i->key);
    free(i->value);
    free(i);
}

void ht_del_hash_table(ht_hash_table* ht){
    // iterate through entire hash table and delete items one-by-one
    for(int i=0; i<ht->size; i++){
        ht_item* item = ht->items[i];
        if(item != NULL){
            ht_del_item(item);
        }
    }
    // delete hash table itself
    free(ht->items);
    free(ht);
}

// convert string to hash
static int ht_hash(const char* s, const int a, const int m){
    long hash = 0;
    const int len_s = strlen(s);
    for(int i=0; i<len_s; i++){
        // s becomes long 
        hash += (long)pow(a, len_s-(i+1))*s[i];
        hash = hash % m; // place int in fixed range
    }
    return (int)hash;
}

// avoid collisions by double hashing
static int get_hash(const char* s, const int num_buckets, const int attempt){
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
}