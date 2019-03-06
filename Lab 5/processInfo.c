
// Matthew Harker
// CS 470
// Lab 5

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "processInfo.h"

// returns a random number from [0..n] (inclusive)
int randomInt(int n)
{
	return rand() % n+1;
}

// initializes a process
void initProcess(ProcessInfo* pi)
{
	pi->state = READY;
	pi->pid = randomInt(INT_MAX);
	pi->priority = randomInt(127); 
	pi->burstTime = randomInt(10);
}

// changes the process's state
void changeProcState(ProcessInfo* pi, Status status)
{
	pi->state = status;
}

// prints the process's state
void printProcState(const ProcessInfo* pi)
{
	if (pi->state == CREATE)
		printf("State: Create\n");
	else if (pi->state == WAITING)
		printf("State: Waiting\n");
	else if (pi->state == EXECUTING)
		printf("State: Executing\n");
	else if (pi->state == READY)
		printf("State: Ready\n");
	else if (pi->state == TERMINATING)
		printf("State: Terminating\n");
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

