
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

void* runProcess(void*);
void* balanceLoad(void*);
void* ageProcesses(void*);
void* randomizePCBStates(void*);

typedef struct {
	PCB* pcb1;
	PCB* pcb2;
} CPU;

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

void assignPCBsToCPU(CPU* cpu, PCB* pcb1, PCB* pcb2)
{
	cpu->pcb1 = pcb1;
	cpu->pcb2 = pcb2;
}

// executes the processes in the two PCBs
void executeProcesses(PCB* pcb1, PCB* pcb2)
{
	pthread_t pcbThread1, pcbThread2, balancer, ager, randomizer;
	
	// check the mutex
	checkMutex();
	
	// reorder the PCBs to follow the correct processing order
	sortByBurstTime(pcb1);
	sortByPriority(pcb2);
	
	CPU cpu = {.pcb1 = pcb1, .pcb2 = pcb2};
	
	while (!isEmpty(pcb1) || !isEmpty(pcb2))
	{
		// create the threads
		pthread_create(&pcbThread1, NULL, runProcess,         (void*)pcb1);
		pthread_create(&pcbThread2, NULL, runProcess,         (void*)pcb2);
		pthread_create(&balancer,   NULL, balanceLoad,        (void*)(&cpu));
		pthread_create(&ager,       NULL, ageProcesses,       (void*)(&cpu));
		pthread_create(&randomizer, NULL, randomizePCBStates, (void*)(&cpu));

		// join all the threads back together
		pthread_join(pcbThread1, NULL);
		pthread_join(pcbThread2, NULL);
		pthread_join(balancer,   NULL);
		pthread_join(ager,       NULL);
		pthread_join(randomizer, NULL);
	}
	
	printf("All process have finished execution\n\n");
}

void* runProcess(void* pcbx)
{
	PCB* pcb = (PCB*)pcbx;

	ProcessInfo* tempPI = &pcb->queue[pcb->front];
	
	int id = tempPI->pid;
	int burstTime = tempPI->burstTime;
	
	if (pcb->front < pcb->size)
	{
		pthread_mutex_lock(&lock);
	
		changeProcState(&pcb->queue[pcb->front], EXECUTING);
		printf("Executing process %d...\n", id);
		sleep(burstTime);
		changeProcState(&pcb->queue[pcb->front], TERMINATING);
		printf("Process finished executing in %d seconds\n\n", burstTime);
	
		pthread_mutex_unlock(&lock);
	
		incrementFront(pcb);
	}
}

// ages the processes every 2 seconds
// won't actually affect much
void* ageProcesses(void* cpuX)
{
	CPU* cpu = (CPU*)cpuX;
	
	// get the two PCBs
	PCB* pcb1 = cpu->pcb1;
	PCB* pcb2 = cpu->pcb2;
	
	sleep(2);
	
	pthread_mutex_lock(&lock);
	
	if (!isEmpty(pcb1))
	{
		printf("\n~~ Aging PCB 1 ~~\n");
		agePCBProcesses(pcb1);
		printf("New priorities for Queue 1\n");
		printAllPriorities(pcb1);
	}
	
	if (!isEmpty(pcb2))
	{
		printf("~~Aging PCB 2 ~~\n");
		agePCBProcesses(pcb2);
		printf("New priorities for PCB 2\n");
		printAllPriorities(pcb2);
	}
	
	printf("\n");
	
	pthread_mutex_unlock(&lock);
	pthread_exit(&lock);
}

// randomizes the PCB's states every 5 seconds
void* randomizePCBStates(void* cpuX)
{
	CPU* cpu = (CPU*)cpuX;
	
	// get the two PCBs
	PCB* pcb1 = cpu->pcb1;
	PCB* pcb2 = cpu->pcb2;
	
	sleep(5);
	
	pthread_mutex_lock(&lock);
	
	if (!isEmpty(pcb1))
	{
		printf("~~ Randomizing PCB 1 states ~~\n");
		randomizeStates(pcb1);
		printf("\n");
	}
		
	if (!isEmpty(pcb2))
	{
		printf("~~ Randomizing PCB 2 states ~~\n");
		randomizeStates(pcb2);
		printf("\n");
	}
	
	printf("\n");
	
	pthread_mutex_unlock(&lock);
	pthread_exit(EXIT_SUCCESS);
}

// when one PCB runsout of processes, take half of the other queue
// will only happen if there are two or more processes in the other PCB
// 	  otherwise the balancer may potentially run forever when there is 1 left
void* balanceLoad(void* cpuX)
{
	CPU* cpu = (CPU*)cpuX;
	
	// get the two PCBs
	PCB* pcb1 = cpu->pcb1;
	PCB* pcb2 = cpu->pcb2;
	
	pthread_mutex_lock(&lock);	
	
	// if pcb1 is empty and pcb2 has more than 1 proc
	if (isEmpty(pcb1) && (pcb2->size - pcb1->front) > 1)
		balancePCBQueues(pcb1, pcb2, 1);
	
	// if pcb2 is empty and pcb1 has more than 1 proc
	else if (isEmpty(pcb2) && (pcb1->size - pcb1->front) > 1)
		balancePCBQueues(pcb2, pcb1, 2);
	
	pthread_mutex_unlock(&lock);
	pthread_exit(EXIT_SUCCESS);
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
	
	PCB pcb1;	// SJF
	PCB pcb2;	// Priority
	
	// initialize the PCBs
	printf("\n~~ Creating PCBS ~~\n");
	
	printf("\nPCB 1: Shortest Job First\n");
	initPCB(&pcb1, m);
	printf("\nPCB 2: Priority\n");
	initPCB(&pcb2, n);
	printf("\n~~ PCBs created ~~\n\n");	
	
	// execute the processes in the PCBs
	executeProcesses(&pcb1, &pcb2);
	
	return 0;
}


