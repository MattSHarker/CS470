
// Matthew Harker
// CS 470
// Lab 5

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "processInfo.h"

#define MAX_BURST_TIME 10
#define MAX_PRIORITY 127

/* for reference

typedef enum {CREATE, WAITING, EXECUTING, READY, TERMINATING} Status;

typedef struct {
	Status state;	// current state of the process
	int pid;		// process ID
	int priority;	// priority 1-127
	int burstTime;	// CPU burst time 0-10
} ProcessInfo;
*/

// returns a random number from [0..n-1] (inclusive)
int randomInt(int n)
{
	return rand() % n;
}

// initializes a process
void initProcess(ProcessInfo* pi)
{
	pi->pid = randomInt(INT_MAX/2);
	changeProcState(pi, CREATE);
	pi->priority = randomInt(MAX_PRIORITY+1); 
	pi->burstTime = (randomInt(MAX_BURST_TIME)+1);
	changeProcState(pi, READY);
}

// changes the process's state
void changeProcState(ProcessInfo* pi, Status status)
{
	pi->state = status;
	printf("Process %d is now: ", pi->pid);
	printProcState(pi);
}

// prints the process's state
void printProcState(const ProcessInfo* pi)
{
	if (pi->state == CREATE)
		printf("CREATE\n");
	else if (pi->state == WAITING)
		printf("WAITING\n");
	else if (pi->state == EXECUTING)
		printf("EXECUTING\n");
	else if (pi->state == READY)
		printf("READY\n");
	else if (pi->state == TERMINATING)
		printf("TERMINATING\n");
}

// prints the process's ID
void printProcID(const ProcessInfo* pi)
{
	printf("ID: %d\n", pi->pid);
}

// prints the process's priority
void printProcPriority(const ProcessInfo* pi)
{
	printf("Priority: %d\n", pi->priority);
}

// prints the process's burst time
void printProcBurstTime(const ProcessInfo* pi)
{
	printf("Burst Time: %d\n", pi->burstTime);
}

// prints all of the process's info
void printProcessInfo(const ProcessInfo* pi)
{
	printProcID(pi);
	printProcState(pi);
	printProcPriority(pi);
	printProcBurstTime(pi);
}

// decrements the process's priority by 1 (creates higher priority)
void decrementPriority(ProcessInfo* pi)
{
	if (pi->priority > 0)
		pi->priority = pi->priority - 1;
}

