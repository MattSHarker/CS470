
// Matthew Harker
// CS 470
// Lab 4

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define clear() printf("\033[H\033[J") 
#define MAX 1024


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void init()
{
	clear();
}

void helpCmd()
{
	printf("This is a list of the following available commands:\n");
	printf("\t1: runs the first problem\n");
	printf("\t2: runs the second problem\n");
	printf("\texit: exits the application\n");
	printf("\thelp: shows the help prompt\n\n");
}

// checks if a char array can be a positive number
// returns 0 if false
// returns 1 if true
int positiveNumber(char* n)
{
	for (int i = 1; i < (unsigned)strlen(n); i++)
	{
		if (!isdigit(n[i]))				// is not a number
		{
			printf("Please ensure you enter a valid positive number\n");
			return 0;
		}
	}
	
	if(!isdigit(n[0]) && n[0] != '-')
	{	
		printf("Please ensure you enter a valid positive number\n");
		return 0;	// false
	}
	else if (atoi(n) < 1)
	{
		printf("Please ensure you enter a positive number\n");
		return 0;
	}
	else
		return 1;
}

// randomly return either 0 or 1
int randomBinary()
{
	return rand() & 1;
	
}

void createBinFile(int n)
{
	int nSquared = n*n;
	
	FILE *file;
	file = fopen("prob1.bin", "wb");	// b for bit mode
	
	for(int i = 0; i < nSquared; i++)
	{
		int x = randomBinary();
		fwrite(&x, sizeof(int), 1, file);
	}
	
	fclose(file);
}

void createTextFile(int n)
{
	int nSquared = n*n;
	int rnd;
	char buff[MAX];
	
	FILE *file;
	file = fopen("prob2.txt", "wb");
	
	for (int i = 0; i < n; i++)
	{
		fprintf(file, "%d", rand() % 1000);
		fprintf(file, "\t");
	}
	
	fprintf(file, "\n");
	
	fclose(file);
}

void printTextFile(int n)
{
	FILE* file;
	char buff[MAX];

	file = fopen("prob2.txt", "r");
	if (file == NULL)
	{
		printf("File could not be opened\n");
		exit(EXIT_FAILURE);
	}

	fscanf(file,"%[^\n]", buff);

	fclose(file);

	char *tok = strtok(buff, "\t");
	int arr[n*n];

	int i = 0;
	while (tok != NULL)
	{
		arr[i++] = atoi(tok);
		tok = strtok(NULL, "\t");
	}

	for (int i = 0; i < n*n; i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
}

// checks if problem 1 has finished (all 0s or all 1s)
int isProbOneDone(int n)
{
	int nSquared = n*n;
	int sum = 0;
	
	FILE *file;
	file = fopen("prob1.bin", "rb");
	
	
	for (int i = 0; i < nSquared; i++)
	{
		int b = 0;
		fread(&b, sizeof(int), 1, file);
		sum += b;
		
		if (i%n == 0)
			printf("\n");
		printf("%d ", b);
	}	
	
	printf("\n\n");
	
	fclose(file);
	
	return (sum == 0 || sum == nSquared);
}

// checks if the file is sorted
int isProbTwoDone(int n)
{
	FILE* file;
	char buff[MAX];

	file = fopen("prob2.txt", "r");
	if (file == NULL)
	{
		printf("File could not be opened\n");
		exit(EXIT_FAILURE);
	}

	fscanf(file,"%[^\n]", buff);

	fclose(file);

	char *tok = strtok(buff, "\t");
	int arr[n*n];

	int i = 0;
	while (tok != NULL)
	{
		arr[i++] = atoi(tok);
		tok = strtok(NULL, "\t");
	}
	
	printArr(n);
	
	for (int i = 0; i < (n*n)-1; i++)
		if (arr[i] > arr[i+1])
			return 0;
	
	return 1;
}

// bubblesort algorithm
void bubbleSort(int* arr, int n)
{
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n-i-1; j++)
			if (arr[j] > arr[j+1])
			{
				int temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;
			}
}

// creates a partition for quicksort
int partition(int arr[], int low, int high)
{
	int pivot = arr[high];
	int i = (low - 1);
	
	for (int j = low; j <= high - 1; j++)
	{
		if (arr[j] <= pivot)
		{
			i++;
			
			int temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
		}
	}
	
	int temp = arr[i+1];
	arr[i+1] = arr[high];
	arr[high] = temp;
	
	return (i + 1);
}

// standard quicksort algorithm
void quickSort(int arr, int low, int high)
{
	if (low < high)
	{
		int partInd = partition(arr, low, high);
		
		quickSort(arr, low, partInd - 1);
		quickSort(arr, partInd + 1, high);
	}
}

// turns an array into a heap
void heapify(int* arr, int n, int i)
{
	int largest = i;
	int left = 2*i + 1;
	int right = 2*i + 2;
	
	if (left < right && arr[left] > arr[largest])
		largest = left;
	
	if (right < n && arr[right] > arr[largest])
		largest = right;
	
	if (largest != i)
	{
		int temp = arr[i];
		arr[i] = arr[largest];
		arr[largest] = temp;

		heapify(arr, n, largest);
	}
}

// standard heapsort algorithm
void heapSort(int* arr, int n)
{
	// build the heap
	for (int i = n/2 - 1; i >= 0; i--)
		heapify(arr, n, i);
	
	// extract elements from the heap
	for (int i = n-1; i >= 0; i--)
	{
		// move root to end
		int temp = arr[0];
		arr[0] = arr[i];
		arr[i] = temp;
		
		// heapify the reduced heap
		heapify(arr, i, 0);
	}
}

int getIndex(int row, int col, int n)
{
	return col + (row * n);
}

void getArr(int* arr, int row, int col, int n)
{
	FILE *file;
	file = fopen("prob1.bin", "rb+");
	
	int nSquared = n*n;
	int sum = 0;
	
	int index = 0;
	
	for (int i = row-1; i < row+1; i++)
	{
		for (int j = col-1; j < col+1; j++)
		{
			int arrIndex = getIndex(i-(row-1), j-(col-1), 3);
			
			if (i >= n || j >= n)
			{
				arr[arrIndex] = -1;
				continue;
			}
			
			int findIndex = getIndex(i, j, n);
			fseek(file, findIndex, SEEK_SET);
			fread(&(arr[arrIndex]), sizeof(int), 1, file);
		}
	}
	
	fclose(file);
}

void changeBit(int bit, int row, int col, int n)
{
	FILE *file;
	file = fopen("prob1.bin", "rb+");
	
	int index = getIndex(row, col, n);
	fseek(file, (long)index*sizeof(int), SEEK_SET);
	fwrite(&bit, sizeof(int), 1, file);
	
	fclose(file);
}

int changeArr(int* arr)
{
	int index = getIndex(1, 1, 3);
	const int size = 9; // 3*3
	
	int sum = 0;
	int dne = 0;	// does not exist, for spots that are not technically on the board
	
	for (int i = 0; i < size; i++)
	{
		if(arr[i] == -1)
		{
			dne++;
			continue;
		}
		
		if (i != index)
			sum += arr[i];
	}
	
	if (2*sum < (size - dne - 1))
		return 0;
	else
		return 1;
}

void* problemOneExec(void* x_void_ptr)
{
	int n = (int *)x_void_ptr;
	pthread_t tid = pthread_self();
	
	int row = rand() % n;
	int col = rand() % n;
	
	int arr[n*n];
	
	pthread_mutex_lock(&lock);
	
	getArr(arr, row, col, n);
	
	int bit = changeArr(arr);
	
	printf("Thread %ld changed (%d, %d) to %d\n", tid, row, col, bit);
	changeBit(bit, row, col, n);
	
	pthread_mutex_unlock(&lock);
	
	pthread_exit(EXIT_SUCCESS);
}


// the first problem
void problemOne(int numThreads, int numElems)
{
	printf("Starting problem one...\n");
	
	createBinFile(numElems);
	
	while(!isProbOneDone(numElems))
	{
		if(pthread_mutex_init(&lock, NULL) != 0)
		{
			printf("mutex init has failed\n");
			exit(EXIT_FAILURE);
		}
	
		pthread_t tid[numThreads];
	
		if (tid == NULL)
		{
			printf("Out of memory\n");
			exit(EXIT_FAILURE);
		}
	
		for (int i = 0; i < numThreads; i++)
		{
			pthread_create(&tid[i], NULL, problemOneExec, (void *)numElems);
		}
	
	
		for (int i = 0; i < numThreads; i++)
			pthread_join(tid[i], NULL);
	
		pthread_mutex_destroy(&lock);
	}
	
	printf("Problem 1 has completed\n\n");
}

void printArr(int n)
{
	FILE *file;
	file = fopen("prob2.txt", "r");
	char buff[MAX];
	int arr[n*n];
	
	if (file == NULL)
	{
		printf("File could not be opened\n");
		exit(EXIT_FAILURE);
	}
	
	fscanf(file, "%[^\n]", buff);
	fclose(file);
	
	char *tok = strtok(buff, "\t");
	
	int i = 0;
	while (tok != NULL)
	{
		arr[i++] = atoi(tok);
		tok = strtok(NULL, "\t");
	}
	
	printf("\n");
	for (int i = 0; i < n*n; i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
}

void* problemTwoExec(void* x_void_ptr)
{	
	pthread_mutex_lock(&lock);
	int n  = (int *)x_void_ptr;
	pthread_t tid = pthread_self();

	FILE* file;
	char buff[MAX];

	file = fopen("prob2.txt", "r");
	if (file == NULL)
	{
		printf("File could not be opened\n");
		exit(EXIT_FAILURE);
	}

	fscanf(file,"%[^\n]", buff);
	fclose(file);

	char *tok = strtok(buff, "\t");
	int arr[n*n];
	

	int i = 0;
	while (tok != NULL)
	{
		arr[i++] = atoi(tok);
		tok = strtok(NULL, "\t");
	}

	// create random numbers
	int low = rand() % (n*n);
	int high = rand() % (n*n +1);
	int sortType = rand() % 3;

	// ensure that low <= high
	if (low > high)
	{
		int temp = low;
		low = high;
		high = temp;
	}

	i = 0;
	int diff = high-low;
	int subArr[diff];

	// get the subarray to sort
	for (int i = 0; i < diff; i++)
	{
		subArr[i] = arr[low + i];
	}

	// sort the subarray
	if (sortType == 0)
		bubbleSort(subArr, diff);
	else if (sortType == 1)
		bubbleSort(subArr, diff);
	else
		bubbleSort(subArr, diff);
	
	// put the reorder subarray back into the array
	int z = 0;
	int newArr[n*n];
	for (int i = 0; i < n*n; i++)
	{
		if (i >= low && i < high)
			newArr[i] = subArr[z++];
		else
			newArr[i] = arr[i];
	}

	printf("Thread %ld sorted (%d, %d) with ", tid, low, high);
	if (sortType == 0)
		printf("bubble sort\n");
	else if (sortType == 1)
		printf("quick sort\n");
	else
		printf("heap sort\n");

	// write the array back into the file
	
	file = fopen("prob2.txt", "w");	// clearing the contents is intentional
	if (file == NULL)
	{
		printf("Failed to open file\n");
		exit(EXIT_FAILURE);
	}
	
	for(int i = 0; i < n*n; i++)
	{
		fprintf(file, "%d\t", newArr[i]);
	}
	fprintf(file, "\n");

	fclose(file);
	
	pthread_mutex_unlock(&lock);
	pthread_exit(EXIT_SUCCESS);
}

// the second problem
void problemTwo(const int numThreads, const int numElems)
{
	printf("\nStarting problem two\n\n");
	
	createTextFile(numElems * numElems);
	
	while(!isProbTwoDone(numElems))
	{
		if(pthread_mutex_init(&lock, NULL) != 0)
		{
			printf("mutex init has failed\n");
			exit(EXIT_FAILURE);
		}
	
		pthread_t *tid;
	
		tid = malloc(sizeof(pthread_t) * numThreads);
	
		if (tid == NULL)
		{
			printf("Out of memory\n");
			exit(EXIT_FAILURE);
		}
	
		for (int i = 0; i < numThreads; i++)
		{
			pthread_create(&tid[i], NULL, problemTwoExec, (void *)numElems);
		}
	
	
		for (int i = 0; i < numThreads; i++)
			pthread_join(tid[i], NULL);
	
		pthread_mutex_destroy(&lock);
	}
	
	printf("Final state of text file:\n");
	printTextFile(numElems);
}


int main(int argc, char** argv)
{
	int numElems;			// number of elements n
	int numThreads;			// number of threads M
	int isRunning = 1;		// when 0 the program will end
	char command[MAX];			// the program to run (or exit or help)
	
	srand(time(NULL));
	
	// ensure the correct amount of commands
	if (argc < 2)
	{
		printf("Not enough commands\n");
		exit(0);
	}
	else if (argc > 2)
	{
		printf("Too many commands\n");
		exit(0);
	}
	
	
	// ensure argv[1] is an int that is above 0
	char* input = argv[1];
	
	if (positiveNumber(input))
		numElems = atoi(argv[1]);
	else
		exit(0);
	
	
	init();
	
	
	while(1)
	{
		// get the command from the user
		printf("Available Commands:\n");
		printf("1) problem 1\n");
		printf("2) problem 2\n");
		printf("3) exit\n");
		printf("Command: ");
		fgets(command, MAX, stdin);		
		command[strcspn(command, "\n")] = 0;	// remove trailing new-line
		
		if (strcmp(command, "help") == 0)
		{
			// display help commands
			helpCmd();
			continue;
		}
		else if(strcmp(command, "1") == 0)	// program 1
		{			
			char temp[MAX];
			
			// get a value for M
			while(1)
			{
				// get value from user
				printf("Enter the amount of threads to use: ");
				fgets(temp, MAX, stdin);
				temp[strcspn(temp, "\n")] = 0;	// remove trailing new-line	
				
				// make sure the number is valid
				if (positiveNumber(temp) == 0)
					continue;
				
				numThreads = atoi(temp);
				
				printf("numThreads: %d\n", numThreads);
				break;
			}
			
			problemOne(numThreads, numElems);
			
		}
		else if (strcmp(command, "2") == 0)	// program 2
		{			
			char temp[MAX];
			
			while(1)
			{
				// get value from user
				printf("Enter the amount of threads to use: ");
				fgets(temp, MAX, stdin);
				temp[strcspn(temp, "\n")] = 0;	// remove trailing new-line	
				
				// make sure the number is valid
				if (positiveNumber(temp) == 0)
					continue;
				numThreads = atoi(temp);
				
				break;
			}
				
			problemTwo(numThreads, numElems);
			
		}
		else if (strcmp(command, "3") == 0 || strcmp(command, "exit"))	// exit the program
		{
			printf("Exiting program\n");
			return 1;
		}
	}
	
	return 1;
}


