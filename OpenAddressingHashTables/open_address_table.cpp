#include "open_address_table.h"

/*
 * function: create_table
 * ----------------------------
 * Creates a hash table using open addressing and lazy deletion
 *	N: table size
 *	hash_function: hash function used to find bucket numbers for keys
 *	compare: function for comparing keys. Must obey the following rules:
 *			compare(a, b) < 0 if a < b
 *			compare(a, b) > 0 if a > b
 *			compare(a, b) = 0 if a == b
 *	record_formatter: returns a C string (char*) representation of a record
 *	key_size: size of keys in bytes
 *	value_size: size of values in bytes
 *
 * returns: a pointer to the table
 *
 */
HashTable* create_table(const int N,
						size_t (*hash_function)(const void*),
						int (*compare)(const void*, const void*),
						char* (*record_formatter)(const void*),
						const size_t key_size, const size_t value_size) {
	// Allocate space for the table
	HashTable* table = (HashTable*) malloc(sizeof(HashTable));

	// Initialize attributes
	table->N = N;
	table->hash_function = hash_function;
	table->compare = compare;
	table->record_formatter = record_formatter;
	table->key_size = key_size;
	table->value_size = value_size;

	// Initialize records - this is an array of Records
	// Initially, all the buckets will be empty
	table->buckets = (Record**) malloc(N * sizeof(Record*));
	for (int i = 0; i < N; i++) {
		table->buckets[i] = NULL;
	}

	return table;
}

/*
 * function: table_to_string
 * ----------------------------
 * Provides a string representation of the table
 *	table: the table to return in string form
 *
 * returns: a string representation of the table
 *
 */
char* table_to_string(const HashTable* table) {
	char* out = (char*) malloc(2048 * sizeof(char));
	sprintf(out, "n\tB[n]\t\n-----------------");

	for (int i = 0; i < table->N; i++) {
        Record* record = table->buckets[i];

        char record_as_string[256];
        if (record == NULL) {
            sprintf(record_as_string, "EMPTY");
        } else if (record == DELETED) {
            sprintf(record_as_string, "DELETED");
        } else {
            char* holder = table->record_formatter(record);
            sprintf(record_as_string, "%s", holder);
            free(holder);
        }

        sprintf(out, "%s\n%d\t%s", out, i, record_as_string);
	}

	return out;
}

/*
 * function: find_bucket
 * ----------------------------
 * Finds the bucket occupied by the entry with a matching key. DELETED buckets
 * are treated as occupied.
 *
 *	table: the table to search
 *  key: key to search for
 *
 * returns: the index of the bucket containing the desired key value
 */
int find_bucket(const HashTable* table, const void* key) {
    if (table == NULL || key == NULL)
    {
        return NOT_FOUND;
    }
	int attempts = 0;
	while (attempts < table->N)
    {
        int i = (table->hash_function(key) + attempts) % table->N;

        if (table->buckets[i] == NULL)
        {
            return NOT_FOUND;
        }
        if (table->buckets[i] == DELETED)
        {
            attempts = attempts + 1;
            continue;
        }
        if (table->buckets[i]->key == key) // seg fault here
        {
            return attempts;
        }

        attempts = attempts + 1;
    }
	return NOT_FOUND;
}

/*
 * function: find_empty_bucket
 * ----------------------------
 * Finds next unoccupied bucket in the table. DELETED buckets are treated as
 * available. Used when inserting a new entry into the table.
 *	table: the table to search
 *  key: key to search for
 *
 * returns: the bucket number if the key is found or -1 if it isn't
 */
int find_empty_bucket(const HashTable* table, const void* key) {
    if (table == NULL || key == NULL)
    {
        return NOT_FOUND;
    }
    int attempts = 0;
    // why does this function have a key to find? should
    // it just find the next empty bucket?
	while (attempts < table->N)
    {
        int i = (table->hash_function(key) + attempts) % table->N;

        if (table->buckets[i] == NULL || table->buckets[i] == DELETED)
        {
            return i;
        }

        attempts = attempts + 1;
    }
	return TABLE_FULL;
}


/*
 * function: insert
 * ----------------------------
 * Inserts a new entry into the table
 *	table: the table to add to
 *  key: key to add
 *  value: corresponding value
 *
 */
bool insert(HashTable* table, const void* key, const void* value) {
    if (table == NULL || key == NULL || value == NULL)
    {
        return false;
    }
	Record* r = create_record(key, table->key_size, value, table->value_size);
	int i = find_empty_bucket(table, key);

	if (i != TABLE_FULL)
    {
        table->buckets[i] = r;
        return true;
	} else
	{
	    return false;
	}
}

/*
 * function: search
 * ----------------------------
 * Searches the table for a particular key and returns the corresponding value
 *	table: the table to search
 *  key: key to search for
 *
 * returns: the value or NULL if key is not in table
 */
void* search(const HashTable* table, const void* key) {
    if (table == NULL || key == NULL)
    {
        return NULL;
    }
	int i = find_bucket(table, key);
	if (i == NOT_FOUND)
	{
	    return NULL;
	} else
	{
	    return table->buckets[i]->value;
	}
	return NULL;
}

/*
 * function: replace
 * ----------------------------
 * Replaces the value of the entry with a given key in the table
 *	table: the table to add to
 *  key: key to add
 *  new_value: new value for key
 *
 */
bool replace(HashTable* table, const void* key, const void* new_value) {
    if (table == NULL || key == NULL || new_value == NULL)
    {
        return false;
    }
	int i = find_bucket(table, key);
	if (i == NOT_FOUND)
    {
        return false;
    } else
    {
        memcpy(table->buckets[i]->value, new_value, table->value_size);
        return true;
    }
}

/*
 * function: remove
 * ----------------------------
 * Removes an entry from the table using lazy deletion
 *	table: the table to remove the entry from
 *  key: key of entry to remove
 *
 */
bool remove(HashTable* table, const void* key) {
    if (table == NULL || key == NULL)
    {
        return false;
    }
	int i = find_bucket(table, key);
	if (i == NOT_FOUND)
    {
        return false;
    } else
    {
        memcpy(table->buckets[i]->value, DELETED, table->value_size);
        return true;
    }
}
