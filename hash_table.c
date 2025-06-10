#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

// set up address for any deleted items to not disrupt collision chain
static ht_item HT_DELETED_ITEM = {NULL, NULL};

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
    return (hash_a + (attempt*(hash_b+1))) % num_buckets; // add 1 to second hash because of collision
}

void ht_insert(ht_hash_table *ht, const char *key, const char *value)
{
    ht_item* item = ht_new_item(key, value);
    int idx = get_hash(item->key, ht->size, 0);
    ht_item* curr = ht->items[idx];
    int i = 1;
    while(curr != NULL && curr != &HT_DELETED_ITEM){
        idx = get_hash(item->key, ht->size, i);
        curr = ht->items[idx];
        i++;
    }
    ht->items[idx] = item;
    ht->count++;
}

char *ht_search(ht_hash_table *ht, const char *key)
{
    int idx = get_hash(key, ht->size, 0);
    ht_item* item = ht->items[idx];
    int i = 1;
    while(item != NULL){
        if(item != &HT_DELETED_ITEM){
            if(strcmp(item->key, key) == 0){
                return item->value; // found item in ht
            }
        }
        // iterate to next item in ht
        idx = get_hash(key, ht->size, i);
        item = ht->items[idx];
        i++;
    }
    // didn't find item
    return NULL;
}

void ht_delete(ht_hash_table *ht, const char *key)
{
    // same process as search
    int idx = get_hash(key, ht->size, 0);
    ht_item* item = ht->items[idx];
    int i=1;
    while(item != NULL){
        if(item != &HT_DELETED_ITEM){
            if(strcmp(item->key, key) == 0){
                ht_del_item(item); // if found, delete
                ht->items[idx] = &HT_DELETED_ITEM;
            }
        }
        idx = get_hash(key, ht->size, i);
        item = ht->items[idx];
        i++;
   }
   ht->count--;
}
