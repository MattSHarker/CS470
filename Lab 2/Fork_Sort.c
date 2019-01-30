
/****************************
		Matthew Harker
		CS 470, Lab 2
 ****************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define DEF_ARR_SIZE 128


// reads in a file to be used later
int readFile(char buff[], char* name)
{
	FILE *fptr;
	fptr = fopen(name, "r");
	
	if (fptr == NULL)
	{
		printf("File was not found\n");
		return 1;
	}
	
	fscanf(fptr, "%[^\n]", buff);
	printf("Original string: %s\n", buff);
	fclose(fptr);
	
	return 0;
}


// prints an array to the console
void printArr(int arr[], int left, int right)
{
	for(int i = left; i < right; i++)
		printf("%d ", arr[i]);
	
	printf("\n");
}


int merge(int arr[], int left, int mid, int right)
{
	// sizes of subarrays
	int lSize = mid - left + 1;
	int rSize = right - mid;
	
	
	// copy data to temp subarrays
	int lArr[lSize], rArr[rSize];
	
	for (int i = 0; i < lSize; i++)
		lArr[i] = arr[left+i];
	
	for (int j = 0; j < rSize; j++)
		rArr[j] = arr[mid + 1 + j];

	
	// print initial status
	printf("Process ID: %u\n", getpid());
	printf("Left subarray:  ");
	printArr(lArr, 0, lSize);
	printf("Right subarray: ");
	printArr(rArr, 0, rSize);
	
	
	// merge temp arrays back 
	int i = 0;		// for left subarray
	int j = 0;		// for right subarray
	int k = left;	// for merged subarray
	
	while (i < lSize && j < rSize)
	{		
		if (lArr[i] <= rArr[j])
		{
			arr[k] = lArr[i];
			i++;
		}
		else
		{
			arr[k] = rArr[j];
			j++;
		}
		
		k++;
	}
	
	// ensures all elements of lArr are copied over
	while (i < lSize)
	{
		arr[k] = lArr[i];
		++i;
		++k;
	}
	
	// ensures all elements of rArr are copied over
	while (j < rSize)
	{
		arr[k] = rArr[j];
		++j;
		++k;
	}
	
	
	// print merged status
	printf("Merged arrays:  ");
	printArr(arr, left, right + 1);
	printf("\n");
}


void mergeSort(int arr[], int left, int right)
{
	// only if there is more that 1 element
    if (left < right)
    {
	    int status;
	    pid_t lchild, rchild;
	    
    	int mid = (left + right)/2;
    	
    	lchild = fork();
    	
    	if (lchild < 0)
    	{
    		perror("fork");
    		exit(1);
    	}
    	if (lchild == 0)
    	{
    		mergeSort(arr, left, mid);
    		exit(0);
    	}
    	else
    	{
	    	rchild = fork();
	    	
	    	if (rchild < 0)
	    	{
	    		perror("fork");
	    		exit(1);
	    	}
	    	if (rchild == 0)
	    	{
    			mergeSort(arr, mid+1, right);
    			exit(0);
	    	}	
    	}
    	
    	// wait for the forks to catch up
    	waitpid(lchild, &status, 0);
    	waitpid(rchild, &status, 0);
    	
    	
    	// merge the subarrays
    	merge(arr, left, mid, right);
    }
}


int main(int argc, char* argv[])
{
	// ensure the right format is entered
	if (argc != 3)
	{
		printf("Command not recognized, please ensure a file name and a delimiter are entered\n");
		exit(0);
	}
	
	
	// read in the file
    char buff[BUFFER_SIZE];
    
    if(readFile(buff, argv[1]) == 1)
    	return 1;
 
    
    // create an array in shared memory
    int *arr;
    int shmid;
    key_t key = IPC_PRIVATE;
    
	if ((shmid = shmget(key, DEF_ARR_SIZE, IPC_CREAT | 0666)) < 0)
	{
		perror("shmget");
		exit(1);
	}
	
    if ((arr = shmat(shmid, NULL, 0)) == (int *)-1)
    {
    	perror("shmat");
    	exit(1);
    }
    
    
    // tokenize the input into the array
    int arrSize = 0;
    
    char *dlm = argv[2];	// the delimiter that determines what seperates the numbers
	char *tok;
	
    tok = strtok(buff, dlm);
    
    while (tok != NULL)
    {
    	arr[arrSize++] = atoi(tok);
    	tok = strtok(NULL, dlm);
    }
    
    
    // sort the array
    printf("Original array: ");
    printArr(arr, 0, arrSize);
    printf("\n");
    
    mergeSort(arr, 0, arrSize-1);
    
    printf("Sorted array: ");
    printArr(arr, 0, arrSize);
    
    
    // finish dealing with the shared memory array
    if (shmdt(arr) == -1)
    {
    	perror("shmdt");
    	exit(1);
    }
    
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
    	perror("shmctl");
    	exit(1);
    }
    
    return 0;
}

