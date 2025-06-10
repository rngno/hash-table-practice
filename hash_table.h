typedef struct{
    char* key;
    char* value;
} ht_item;

typedef struct{
    int size;
    int base_size;
    int count;
    ht_item** items;
} ht_hash_table;

void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);
static void ht_resize_up(ht_hash_table* ht); // have to declare these here for formatting purposes in hash_table.c
static void ht_resize_down(ht_hash_table* ht); 