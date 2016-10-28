// Stack ADT interface

// ##########################
// STACK REP AS A LINKED LIST
// ##########################

// Items = char

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Stack.h"

typedef char Item;
typedef struct Node *Link;
typedef struct Node { Item value; Link next; } Node;
typedef struct StackRep { int nitems; Link items; } StackRep;

// Stack validity check
static int isValidStack(Stack s)
{
	if (s == NULL) return 0;		// empty stack, not valid
	if (s->nitems < 0) return 0;	// nitems < 0, not valid
	if (s->nitems > 0 && s->items == NULL) return 0;	// nitems = null, not valid
	Link cur; int nitems = 0;
	for (cur = s->items; cur != NULL; cur = cur->next)	// loop through stack
		nitems++;											// count items in stack
	if (nitems != s->nitems) return 0;					// if nitems doesn't match count, not valid

	return 1;	// passed call conditions, valid stack.
}

// add item onto stack
// Pre-condition: valid(Stack,s) // && !isFull(s)
// Post-condition: s' == it:s 	(s' = new value of stack at end of the FN)
void push(Stack s, Item it)
{
	assert(isValidStack(s));
	
	Link new = malloc(sizeof(Node));
	new->value = it;
	new->next = s->items;
	s->items = new;
	s->nitems++;
}
 
// remove item from stack
// Pre: valid(Stack,s) && nItems(s) > 0 && s == x:ss
// Post: ret == x && s' == ss
Item pop(Stack s)
{
	assert(isValidStack(s));

	Link old = s->items;
	s->items = old->next;
	Item it = old->value;
	s->nitems--;
	free(old);
	return (it);
}
 
// stack contains no items?
// Pre: valid(Stack,s)
// Post: ret == (nItems(s) == 0)
int isEmpty(Stack s)
{
	return (nItems(s) == 0);
}
 
// how many items in stack
int nItems(Stack s)
{
	return s->nitems;
}
 
// display stack on stdout
void show(Stack s)
{
	printf("top: ");
	Link cur;
	for (cur = s->items; cur != NULL; cur = cur->next)
		printf("%c ",cur->value);
	printf("\n");
}
 
// create new empty stack
// Pre: nothing
// Post valid(Stack,ret)
Stack newStack()
{
	Stack new = malloc(sizeof(struct StackRep));
	assert(new != NULL);
	new->nitems = 0;
	new->items = NULL;
	return new;
}
 
// release stack data
void dropStack(Stack s)
{
	Link cur, next;
	for (cur = s->items; cur != NULL; cur = next) {
		next = cur->next;
		free(cur);
	}
	free(s);
}
 
