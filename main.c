#include "hash_table.h"

int main(){
    // test to make sure ht_new() and ht_del_hash_table() work
    ht_hash_table* ht = ht_new();
    ht_del_hash_table(ht);
}