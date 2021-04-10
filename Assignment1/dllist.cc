#include "stdio.h"
#include "stdlib.h"
#include "assert.h"
#include "dllist.h"
#include "thread.h"

extern Thread *currentThread;
extern int threadTestcase;

// initialize a list element (constructor)
DLLElement::DLLElement(void *itemPtr, int sortKey)
{
	item = itemPtr;
	key = sortKey;
	next = NULL;
	prev = NULL;
}

DLList::DLList()
{
	first = NULL;
	last = NULL;
}

DLList::~DLList()
{
	DLLElement *ptr;
	while (first != NULL) {
		ptr = first;
		first = first->next;
		delete ptr;
	}
	last = NULL;
}

// add to head of list, set key = min_key - 1
void DLList::Prepend(void *item)
{
	DLLElement *curItem = new DLLElement(item, (first == NULL) ? 0 : (first->key - 1));
	curItem->next = first;
	if (first != NULL)
		first->prev = curItem;
	
	if (threadTestcase == 1 || threadTestcase == 3)
		currentThread->Yield();

	first = curItem;
	last = (last == NULL) ? curItem : last;
}

// add to tail of list, set key = max_key + 1
void DLList::Append(void *item)
{
	DLLElement *curItem = new DLLElement(item, (last == NULL) ? 0 : (last->key + 1));
	curItem->prev = last;
	if (last != NULL)
		last->next = curItem;
	last = curItem;
	first = (first == NULL) ? curItem : first;
}

// remove from head of list
// set *keyPtr to key of the removed item
// return item (or NULL if list is empty)
void* DLList::Remove(int *keyPtr)
{
	if (first == NULL)
		return NULL;
	*keyPtr = first->key;
	// get first remove element
	DLLElement *removedItem = first;

	if (threadTestcase == 4) {
		printf("Prepare to remove: %d\n", first->key);
		currentThread->Yield();
	}

	// set first & last pointer
	last = (first == last) ? NULL : last;
	first = first->next;
	if (first != NULL)
		first->prev = NULL;

	void* element = removedItem->item;
	delete removedItem;
	return element;
}

// return true if list has elements
bool DLList::IsEmpty()
{
	return (first != NULL && last != NULL);
}

// routines to put/get items on/off list in order (sorted by key)
void DLList::SortedInsert(void *item, int sortKey)
{
	if (first == NULL || sortKey <= first->key) {
		Prepend(item);
		first->key = sortKey;
		return;
	}
	
	DLLElement *newItem = new DLLElement(item, sortKey),
			   *ptr = first;
	while (ptr != NULL && ptr->key < sortKey)
		ptr = ptr->next;
	if (ptr == NULL) {
		last->next = newItem,
		newItem->prev = last,
		last = newItem;
	}
	else {
		if (threadTestcase == 2)
			currentThread->Yield();
		ptr->prev->next = newItem;
		newItem->prev = ptr->prev;
		newItem->next = ptr;
		ptr->prev = newItem;
		if (threadTestcase == 2)
			currentThread->Yield();
	}
}

void* DLList::SortedRemove(int sortKey)
{
	DLLElement *ptr = first;
	assert(first != NULL);
	while (ptr != NULL && ptr->key < sortKey)
		ptr = ptr->next;
	if (ptr == NULL || ptr->key != sortKey)
		return NULL;
	if (ptr->next != NULL)
		ptr->next->prev = ptr->prev;
	if (ptr->prev != NULL)
		ptr->prev->next = ptr->next;
	return ptr->item;
}

void DLList::foreach(bool showRelation)
{
	int sz = 0, flag = 0;
	DLLElement *ptr = first;
	printf("\t   ");

	while (ptr != NULL) {
		flag = 1;
		if (sz != 0)
			printf("->");
		printf("%d", ptr->key);
		if (showRelation) {
			printf("(");
			if (ptr->prev != NULL)
				printf("p=%d", ptr->prev->key);
			if (ptr->prev != NULL && ptr->next != NULL)
				printf(" ");
			if (ptr->next != NULL)
				printf("n=%d", ptr->next->key);
			printf(")");
		}
		sz++, ptr = ptr->next;
	}
	if (flag)
		putchar(' ');
	printf("(total: %d)\n", sz);
}
