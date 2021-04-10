// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "stdlib.h"
#include "time.h"
#include "copyright.h"
#include "system.h"
#include "dllist.h"

// testnum is set in main.cc
int testnum = 1;
// numberCount & threadCount is set in main.cc by reading arguments
int numberCount = 10;
int threadCount = 2;
// thread error testcase number
int threadTestcase = 0;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
	
    DEBUG('t', "Entering ThreadTest1");
    Thread *t = new Thread("forked thread");
    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

DLList *list;


void DLListThread(int threadNum)
{
	switch (threadTestcase) {
		case 2:
			printf("[Thread %d] Inserted %d\n", threadNum, 3 - threadNum);
			list->SortedInsert((void*)0, 3 - threadNum);
			list->foreach(threadNum);
			break;
		case 3:
			if (threadNum == 0) {
				printf("[Thread %d] Inserted %d\n", threadNum, 0);
				list->SortedInsert((void*)0, 0);
			}
			else if (threadNum == 1) {
				int key;
				list->Remove(&key);
				printf("[Thread %d] Removed first element: %d\n", threadNum, key);
				currentThread->Yield();
			}
			if (threadNum == 0)
				list->foreach(0);
			break;
		case 4:
			printf("[Thread %d] ", threadNum);
			int key;
			list->Remove(&key);
			break;
		default:
			PutItem(list, numberCount, threadNum);
			RemoveItem(list, numberCount, threadNum);
			break;
	};
}


void ThreadTest2()
{
	DEBUG('t', "Entering ThreadTest2");
	DEBUG('t', "Creating DLList()");
	list = new DLList();

	// initialize link-list for different testcase
	switch (threadTestcase) {
		case 2:
		case 4:
			list->SortedInsert((void*)0, 1);
			list->SortedInsert((void*)0, 4);
			printf("[Event] Before insert / remove:\n");
			list->foreach(0);
			break;
		case 3:
			list->SortedInsert((void*)0, 1);
			list->SortedInsert((void*)0, 2);
			printf("[Event] Before thread-0 insert, thread-1 remove:\n");
			list->foreach(0);
			break;
	}

	DEBUG('t', "Creating fork thread");
	for (int i = 1; i < threadCount; i++) {
		Thread *t = new Thread("fork thread");
		t->Fork(DLListThread, i);
	}
	
	DLListThread(0);

	if (list != NULL)
		delete list;
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
	case 2:
	ThreadTest2();
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}

