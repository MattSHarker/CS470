
// Matthew Harker
// CS 470
// Lab 5

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "pcb.h"
#include "processInfo.h"

/* for reference
typedef struct {
	ProcessInfo* queue;	// all of the processes in the Processor
	int size;			// size of the queue
	int front;			// front of the queue
}	PCB;
*/

// initializes a PCB
void initPCB(PCB* pcb, int size)
{
	// set up the queue of processes
	pcb->queue = (ProcessInfo *) malloc(sizeof(ProcessInfo) * size);
	
	// set up the size and front
	pcb->size = size;
	pcb->front = 0;
	
	// create each process
	for (int i = 0; i < size; i++)
		initProcess(&pcb->queue[i]);
}

// destructs a PCB
void destructPCB(PCB* pcb)
{
	free(pcb->queue);
}

// compares the priorities of two processes
int priorityCmp(const void* p1, const void* p2)
{
	int x = ((ProcessInfo*)p1)->priority;
	int y = ((ProcessInfo*)p2)->priority;
	
	return (x-y);
}

// compares the burst times of two processes
int burstTimeCmp(const void* p1, const void* p2)
{
	int x = ((ProcessInfo*)p1)->burstTime;
	int y = ((ProcessInfo*)p2)->burstTime;
	
	return (x-y);
}

// sorts a PCB by priority (ascending order)
void sortByPriority(PCB* pcb)
{
	qsort(pcb->queue, pcb->size, sizeof(ProcessInfo), priorityCmp);
}

// sorts a PCB by burst time (ascending order)
void sortByBurstTime(PCB* pcb)
{
	qsort(pcb->queue, pcb->size, sizeof(ProcessInfo), burstTimeCmp);
}

// randomizes one process's status (ready/waiting)
void randomizeStates(PCB* pcb)
{
	int size = pcb->size;
	
	// goes through every process and randomly changes its state 
	for (int i = 0; i < size; i++)
	{
		if (rand() % 2)
			changeProcState(&pcb->queue[i], READY);
		else
			changeProcState(&pcb->queue[i], WAITING);
	}
}

// sets the size of the PCB
void setPCBSize(PCB* pcb, int n)
{
	pcb->size = n;
}

// decreases the size of the PCB by one
void decrementPCBSize(PCB* pcb)
{
	if (pcb->size > 0)
		pcb->size = pcb->size - 1;
}

// increases the size of the PCB by one
void incrementPCBSize(PCB* pcb)
{
	pcb->size = pcb->size + 1;
}

// moves the start of the queue ahead one
void incrementFront(PCB* pcb)
{
	pcb->front = pcb->front + 1;
}


// ages all of the processes in the PCB
void ageProcesses(PCB* pcb)
{
	for (int i = 0; i < pcb->size; i++)
		decrementPriority(&pcb->queue[i]);
}

// prints out the PCB
void printPCB(PCB* pcb)
{
	for (int i = 0; i < pcb->size; i++)
	{
		printProcessInfo(&pcb->queue[i]);
		printf("\n");
	}
}


