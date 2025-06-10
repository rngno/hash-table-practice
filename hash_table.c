#include <stdlib.h>
#include <string.h>
#include "hash_table.h"

// static vars to be used elsewhere as bounds for the hash table
const static int HT_PRIME_1 = 1;
const static int HT_PRIME_2 = 2;
const static int HT_INITIAL_BASE_SIZE = 53;

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
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static ht_hash_table* ht_new_sized(const int base_size){
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht->base_size = base_size;
    ht->size = next_prime(ht->base_size);
    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
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
    // check if new item will exceed ht's current capacity
    const int load = ht->count * 100 / ht->size;
    if(load > 70){
        ht_resize_up(ht);
    }

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
    // check if ht is too big
    const int load = ht->count * 100 / ht->size;
    if(load < 10){
        ht_resize_down(ht);
    }

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

static void ht_resize(ht_hash_table* ht, const int base_size){
    // check if trying to resize under our min
    if(base_size < HT_INITIAL_BASE_SIZE){
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for(int i=0; i<ht->size; i++){
        ht_item* item = ht->items[i];
        // only delete if not NULL
        if(item != NULL && item != &HT_DELETED_ITEM){
            ht_insert(new_ht, item->key, item->value);
        }
    }

    // swap old and new ht, then delete the new one
    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}

// separated for ease of use
static void ht_resize_up(ht_hash_table* ht){
    const int new_size = ht->base_size*2;
    ht_resize(ht, new_size);
}

// separated for ease of use
static void ht_resize_down(ht_hash_table* ht){
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}