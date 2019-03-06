
// Matthew Harker
// CS 470
// Lab 5

#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "processInfo.h"
#include "pcb.h"


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void checkArgAmount(int);
void executeProcesses(PCB*, PCB*);
void checkMutex();
void checkPThread(pthread_t*);

int cliToInt(char*);

void* execPCB1(void*);
void* execPCB2(void*);


// ensures the correct amount of arguments were entered
void checkArgAmount(int argc)
{
	if (argc > 3)
	{
		printf("Too many commands entered\n");
		printf("Format should be <command> <m> <n>\n");
		exit(EXIT_FAILURE);
	}
	else if (argc < 3)
	{
		printf("Not enough commands entered\n");
		printf("Format should be <command> <m> <n>\n");
		exit(EXIT_FAILURE);
	}
}

// checks if the mutex was correctly set up
void checkMutex()
{
	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("Mutex init has failed\n");
		exit(EXIT_FAILURE);
	}
}

// checks if the pthread was correctly set up
void checkPThread(pthread_t* pthr)
{
	if (pthr == NULL)
	{
		printf("Out of memory\n");
		exit(EXIT_FAILURE);
	}
}

// takes in a command line argument and checks if it can be a positive int > 0
int cliToInt(char* argv)
{
	// check for any non-digits
	for (int i = 1; i < (unsigned)strlen(argv); i++)
	{
		if (!isdigit(argv[i]))				// is not a number
		{
			printf("Please ensure you enter valid positive numbers\n");
			exit(EXIT_FAILURE);
		}
	}
	
	if (atoi(argv) < 1)
	{
		printf("Please ensure you enter positive numbers\n");
		exit(EXIT_FAILURE);
	}
	
	return atoi(argv);
}

// executes the processes in the two PCBs
void executeProcesses(PCB* pcb1, PCB* pcb2)
{
	
	pthread_t thread1, thread2, manager;
	
	// check the mutex
	checkMutex();
	
	// create the threads
	pthread_create(&thread1, NULL, execPCB1, (void*)pcb1);
	pthread_create(&thread2, NULL, execPCB2, (void*)pcb2);
	
	// join all the threads back together
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
}

// executes the processes in PCB1
// utilizes SJF (Shortest Job First) scheduling
void* execPCB1(void* pcbx)
{
	PCB* pcb = (PCB*)pcbx;
	
	printf("~~ Original PCB1 order ~~\n");
	printPCB(pcb);
	
	sortByBurstTime(pcb);
	
	printf("~~ New PCB1 order ~~\n");
	printPCB(pcb);
	
}

// executes the processes in PCB2
// utilizes priority scheduling
void* execPCB2(void* pcbx)
{
	PCB* pcb = (PCB*)pcbx;
	
	printf("~~ Original PCB2 order ~~\n");
	printPCB(pcb);
	
	sortByPriority(pcb);
	
	printf("~~ New PCB2 order ~~\n");
	printPCB(pcb);
}

// main 
int main(int argc, char** argv)
{
	int m;	// # processes for PCB1
	int n;	// # processes for PCB2
	srand(time(NULL));
	
	// ensure the correct amount of arguments are used
	checkArgAmount(argc);
	
	// get the process amounts from the command line
	m = cliToInt(argv[1]);
	n = cliToInt(argv[2]);	
	printf("m: %d\n", m);
	printf("n: %d\n\n", n);
	
	PCB pcb1;	// SJF
	PCB pcb2;	// Priority
	
	// initialize the PCBs
	puts("Creating PCBS");
	initPCB(&pcb1, m);
	initPCB(&pcb2, n);
	puts("PCBs created\n");	
	
	// execute the processes in the PCBs
	executeProcesses(&pcb1, &pcb2);
	
	// free allocated memory
	destructPCB(&pcb1);
	destructPCB(&pcb2);
	
	return 0;
}


