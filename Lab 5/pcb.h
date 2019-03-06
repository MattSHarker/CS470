
// Matthew Harker
// CS 470
// Lab 5

#ifndef PCB_H_
#define PCB_H_

#include "processInfo.h"

typedef struct {
	ProcessInfo* queue;	// queue of processes
	int size;			// size of the queue
	int front;			// front of the queue
}	PCB;

void initPCB(PCB*, int);
void destructPCB(PCB*);

int priorityCmp(const void*, const void*);
int burstTimeCmp(const void*, const void*);
void sortByPriority(PCB*);
void sortByBurstTime(PCB*);

void randomizeStates(PCB*);
void ageProcesses(PCB*);
void setPCBSize(PCB*, int);
void decrementPCBSize(PCB*);
void incrementPCBSize(PCB*);
void incrementFront(PCB*);
void printPCB(PCB*);



#endif