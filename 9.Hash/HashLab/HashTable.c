// HashTable.c ... implementation of HashTable ADT
// Written by John Shepherd, May 2013

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "HashTable.h"
#include "List.h"

typedef enum {
	SEPARATE_CHAINS,
	LINEAR_PROBING,
	DOUBLE_HASHING
} CollType;

typedef enum {
	OCCUPIED,
	NO_ITEM,
	DELETED
} State;

typedef struct HashTabRep {
	CollType tabType;
	List *lists;  // either use this	// Array of lists -> for Separate Chaining
	Item *items;  // or use this		// Array of items -> for normal HT
	State *state; // ... and this
	int   nslots; // # elements in array
	int   nitems; // # items stored in HashTable
	int   nhash2; // mod for 2nd hash 	// MOD # FOR SECONDARY HASH
} HashTabRep;


// hash function
int hash(Key k, int N)
{
	int h = 0;  char *c;
	int a = 127; // a prime number
	for (c = k; *c != '\0'; c++)
		h = (a * h + *c) % N;
	return h;
}

// create an empty HashTable
HashTable newHashTable(int N, char t, int N2)
{
	int i;
	HashTable new = malloc(sizeof(HashTabRep));
	assert(new != NULL);
	new->nslots = N;
	new->nitems = 0;
	switch (t) {
	case 'C':								 // #1 SEPARATE CHAINING
		new->tabType = SEPARATE_CHAINS;		 //    Allocate array of lists
		new->lists = malloc(N*sizeof(List)); //    rather than array of items
		for (i = 0; i < N; i++)
			new->lists[i] = newList();
		break;
	case 'L':									
	case 'D':
		if (t == 'L')							// Collision Resolution Strat #2
			new->tabType = LINEAR_PROBING;
		else 									// Collision Resolution Strat #3
			new->tabType = DOUBLE_HASHING;
		Item *a = malloc(N*sizeof(Item));
		State *s = malloc(N*sizeof(State));
		assert(a != NULL && s != NULL);
		for (i = 0; i < N; i++)
			s[i] = NO_ITEM;
		new->items = a; new->state = s;
		new->nhash2 = N2;
		break;
	}
	return new;
}

// free memory associated with HashTable
void dropHashTable(HashTable ht)
{
	assert(ht != NULL);
	switch (ht->tabType) {
	case SEPARATE_CHAINS:
		break;
	case LINEAR_PROBING:
		break;
	case DOUBLE_HASHING:
		break;
	}
}

// display HashTable stats
void showHashTable(HashTable ht)
{
	assert(ht != NULL);
	int i, N = ht->nslots;
	switch (ht->tabType) {
	case SEPARATE_CHAINS:
		for (i = 0; i < N; i++) {
			printf("[%02d] ",i);
			showList(ht->lists[i]);
			putchar('\n');
		}
		break;
	case LINEAR_PROBING:
	case DOUBLE_HASHING:
		for (i = 0; i < N; i++)
			printf(" [%02d]",i);
		putchar('\n');
		for (i = 0; i < N; i++) {
			switch (ht->state[i]) {
			case NO_ITEM: printf("%5s","-"); break;
			case DELETED: printf("%5s","/"); break;
			case OCCUPIED: showItem(ht->items[i]); break;
			}
		}
		putchar('\n');
		break;
	}
}

// #######################
// DIFFERENT INSERT STYLES
// #######################

void insertChain(HashTable ht, Item it)
{
	int N = ht->nslots;
	int h = hash(key(it),N);
	ListInsert(ht->lists[h],it);
}

void insertLinear(HashTable ht, Item it)
{
   assert(ht->nitems < ht->nslots);
   int N = ht->nslots;
   Item *a = ht->items;
   State *s = ht->state;
   Key k = key(it);
   int i, j, h = hash(k,N);
   for (j = 0; j < N; j++) {		// Iterate until FREE SLOT
      i = (h+j)%N;						// Wrap around to start of array
      if (s[i] == NO_ITEM) break;	// Free slot found, break
      if (eq(k,key(a[i]))) break;	// Same key value, break
   }
   if (s[i] != OCCUPIED) ht->nitems++;
   a[i] = copyItem(it);
   s[i] = OCCUPIED;
}

void insertDouble(HashTable ht, Item it)
{
}

// insert a new value into a HashTable
void HashTableInsert(HashTable ht, Item it)
{
	assert(ht != NULL);
	printf("hash(%s) = %d\n", key(it), hash(key(it),ht->nslots));
	switch (ht->tabType) {
	case SEPARATE_CHAINS:
		insertChain(ht,it);
		break;
	case LINEAR_PROBING:
		insertLinear(ht,it);
		break;
	case DOUBLE_HASHING:
		break;
	}
}

// #######################
// DIFFERENT DELETE STYLES
// #######################

void deleteChain(HashTable ht, Key k)
{
	int N = ht->nslots;
	int h = hash(k,N);
	ListDelete(ht->lists[h],k);
}

void deleteLinear(HashTable ht, Key k)
{
   int N = ht->nslots;
   Item *a = ht->items;
   State *s = ht->state;
   int i, j, h = hash(k,N);
   for (j = 0; j < N; j++) {		// Find item to delete
      i = (h+j)%N;
      if (s[i] == NO_ITEM) return;	// Reach end of linear probing chain
      if (eq(k,key(a[i]))) break;
   }
   if (s[i] == OCCUPIED) ht->nitems--;	// decrement nitems
   s[i] = DELETED;						// delete item
}

void deleteDouble(HashTable ht, Key k)
{
}

// delete a value from a HashTable
void HashTableDelete(HashTable ht, Key k)
{
	assert(ht != NULL);
	switch (ht->tabType) {
	case SEPARATE_CHAINS:
		deleteChain(ht,k);
		break;
	case LINEAR_PROBING:
		deleteLinear(ht,k);
		break;
	case DOUBLE_HASHING:
		break;
	}
}

// #######################
// DIFFERENT SEARCH STYLES
// #######################

// Search using SEPARATE CHAINING strat
Item *searchChain(HashTable ht, Key k)
{
	int h = hash(k,ht->nslots);
	return ListSearch(ht->lists[h],k);
}

// Search using LINEAR strat
Item *searchLinear(HashTable ht, Key k)
{
   int N = ht->nslots;
   Item *a = ht->items;
   State *s = ht->state;
   int i, j, h = hash(k,N);
   for (j = 0; j < N; j++) {
      i = (h+j)%N;
      if (s[i] == NO_ITEM) return NULL;
      if (s[i] != DELETED && eq(k,key(a[i]))) return &(a[i]);
   }
   return NULL;
}

// Search function for DOUBLE HASHING
Item *search(HashTable ht, Key k)
{
	int N = ht->nslots;
	int i, j, h = hash(k,N);
	int incr = hash(k,ht->nhash2)+1;

	for (j = 0; j < N; j++) {
		if (eq(k,key(ht->items[i]) == 0))
			return &(ht->items[i]);
		i = (i+incr)%N;
	}
	return NULL;
}


// get Item from HashTable using Key
Item *HashTableSearch(HashTable ht, Key k)
{
	assert(ht != NULL);
	Item *ip = NULL;
	switch (ht->tabType) {
	case SEPARATE_CHAINS:
		ip = searchChain(ht,k);
		break;
	case LINEAR_PROBING:
		ip = searchLinear(ht,k);
		break;
	case DOUBLE_HASHING:
		break;
	}
	return ip;
}

