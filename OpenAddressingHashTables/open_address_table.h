#ifndef OPEN_ADDRESS_H_INCLUDED
#define OPEN_ADDRESS_H_INCLUDED

#include "record.h"
#include <stdio.h>
#include <string>

/*
 * struct: HashTable
 * ----------------------------
 * A hash table using open addressing and lazy deletion
 *	N: table size
 *	buckets: array of pointers to records
 *	hash_function: hash function used to find bucket numbers for keys
 *	compare: function for comparing keys. Must obey the following rules:
 *			compare(a, b) < 0 if a < b
 *			compare(a, b) > 0 if a > b
 *			compare(a, b) = 0 if a == b
 *	record_formatter: returns a C string (char*) representation of a record
 *	key_size: size of keys in bytes
 *	value_size: size of values in bytes
 *
 */
typedef struct {
	int N;
	Record** buckets;
	size_t (*hash_function)(const void*);
	int (*compare)(const void*, const void*);
	char* (*record_formatter)(const void*);
	size_t key_size;
	size_t value_size;
} HashTable;

HashTable* create_table(const int,
						size_t (const void*),
						int (const void*, const void*),
						char* (const void*),
						const size_t, const size_t);

char* table_to_string(const HashTable*);
bool insert(HashTable*, const void*, const void*);
void* search(const HashTable*, const void*);
bool replace(HashTable*, const void*, const void*);
bool remove(HashTable*, const void*);

// Special helper functions for this table type
int find_bucket(const HashTable*, const void*);
int find_empty_bucket(const HashTable*, const void*);

// Used for lazy deletion
Record* const DELETED = (Record*) malloc(sizeof(Record));


#endif
