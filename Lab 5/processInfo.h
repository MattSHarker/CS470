
// Matthew Harker
// CS 470
// Lab 5

#ifndef PROCESS_INFO_H_
#define PROCESS_INFO_H_

typedef enum {CREATE, WAITING, EXECUTING, READY, TERMINATING} Status;

typedef struct {
	Status state;	// current state of the process
	int pid;		// process ID
	int priority;	// priority 1-127
	int burstTime;	// CPU burst time 0-10
} ProcessInfo;

int randomInt(int);
void initProcess(ProcessInfo*);
void changeProcState(ProcessInfo*, Status);

void printProcState(const ProcessInfo*);
void printProcID(const ProcessInfo*);
void printProcPriority(const ProcessInfo*);
void printProcBurstTime(const ProcessInfo*);
void printProcessInfo(const ProcessInfo*);

void decrementPriority(ProcessInfo*);
void balanceLoads(ProcessInfo, ProcessInfo*);

#endif	//PROCESS_INFO_H_
