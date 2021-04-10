#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "system.h"
#include "dllist.h"

bool isSranded = false;

void PutItem(DLList *list, int n, int threadNumber)
{
	if (!isSranded) {
		isSranded = true;
		srand(time(NULL));
	}
	for (int i = 0; i < n; i++) {
		int key = rand() % 1000;
		printf("[Thread %d] insert: %d\n", threadNumber, key);
		list->SortedInsert((void*)key, key);
		list->foreach(0);

		currentThread->Yield();
	}
}

void RemoveItem(DLList *list, int n, int threadNumber)
{
	for (int i = 0, key; i < n; i++) {
		printf("[Thread %d] remove: ", threadNumber);
		list->Remove(&key);
		printf("%d\n", key);
		list->foreach(0);

		currentThread->Yield();
	}
}

