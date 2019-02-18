
// Matthew Harker
// CS 470
// server side

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define MAX 128
#define PORT 5457
#define SA struct sockaddr


int remainingSeats;
int sockfd;

typedef struct
{
	int row;
	int col;
} Seat;

// randomize an array
void randomizeArray(int** arr, int n)
{
	struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);

    if (n > 1)
    {
        size_t i;
        for (i = n - 1; i > 0; i--)
        {
            size_t j = (unsigned int) (drand48()*(i+1));
            int t = *arr[j];
            *arr[j] = arr[i];
            *arr[i] = t;
        }
    }
}

// buy a single seat
void buySeat(int** seats, int row, int col)
{
	
	if (seats[row][col] == 0)
	{		
		char* msg = "Seat ";
		char* tmp;
		
		snprintf(tmp, 128, row);
		strcat(msg, tmp);
		strcat(msg, " ");
		
		snprintf(tmp, 128, col);
		strcat(msg, tmp);
		strcat(msg, " has been purchased\n");
		
		write(sockfd, msg, sizeof(msg));
		remainingSeats--;
	}
	else
	{
		char* msg = "Seat has already been purchased, select another one\n";
		write(sockfd, msg, sizeof(msg));
	}
}

// buy all the seats in a random order
void buyRandomSeats(int** seats, int row, int col)
{
	int size = row*col;
	int allSeats[size][2];
	
	// create all of the seats
	int tmp = 0;
	for (int i = 0; i < row; i++)
	{
		for(int j = 0; j < col; j++)
		{
			allSeats[tmp][0] = i;
			allSeats[tmp++][1] = j;
		}
	}
	
	randomizeArray(allSeats, size);
	
	int i = size;
	int j = 0;
	while (i > 0)
	{
		int tmpRow = allSeats[j][0];
		int tmpCol = allSeats[j++][1];
		
		if (seats[tmpRow][tmpCol] = 0)
		{
			buySeat(seats, tmpRow, tmpCol);
		}
		i--;
		sleep(1);
	}
}



// prints out the current map of seats
// 0 for available seats
// 1 for purchased seats
void displaySeats(int sockfd, int** seats, int row, int col)
{
	char msg[4098];
	char tmp[4098];
	printf("Map of seats:\n");
	msg = "Map of seats:\n";

	for (int i = 0; i < row; i++)
	{
		
		for (int j = 0; j < col; j++)
		{
		puts("asd2");
			snprintf(tmp, 128, seats[i][j]);
			strcat(msg, tmp);
			printf("%i ", seats[i][j]);
		}
		
		strcat(msg, "\n");
		printf("\n");
	}
	
	write(sockfd, msg, sizeof(msg));
}

void runServer(int sockfd, int** seats, int row, int col)
{
	char buff[MAX];
	int n;
	
	while (1)
	{
		displaySeats(sockfd, seats, row, col);
		
		// get the info from the client
		bzero(buff, MAX);
		read(sockfd, buff, sizeof(buff));
		
		char* tok;
		char* temp;
		int i = 0;
		Seat seat;
		
		// get the input
		temp = strtok(buff, " ");
		tok[i++] = temp;
		
		while (temp!= NULL)
		{
			temp = strtok(NULL, " ");
			tok[i++] = temp;
		}
		
		// the wrong amount of commands
		if (i > 2)
			continue;
		else if (i == 0)
		{
			write(sockfd, "\n", sizeof("\n"));
			continue;
		}
		
		// buy a specific seat
		else if (i == 2)
		{
			seat.row = atoi(tok[0]);
			seat.col = atoi(tok[1]);
			if (seat.row == 0 || seat.col == 0)
			{
				char* message = "Please enter valid numbers";
				write(sockfd, message, sizeof(message));
				continue;
			}
			else
			{
				buySeat(seats, seat.row, seat.col);
			}
		}
		// buy random seats
		else if (i == 1)
		{
			char* rnd = "random";
			char* ext = "exit";
			char* help = "help";
			
			if (strcmp(buff, rnd))
			{
				buyRandomSeats(seats, row, col);
				
			}
			else if (strcmp(buff, ext))
			{
				printf("Exiting...");
				exit(0);
			}
			else if (strcmp(buff, help))
			{
				char* helpMsg = "To buy a seat: <row> <col>\nTo buy all seats randomly: random\nTo exit the program: exit\n";
				write(sockfd, helpMsg, sizeof(helpMsg));
				continue;
			}
		}

	}
}

int main(int argc, int* argv[])
{	
	int row, col;
	
	if (argc == 1)
	{
		row = 10;
		col = 10;
	}
	else if (argc == 2)
	{
		row = atoi(argv[1]);
		col = atoi(argv[1]);
		
		if (row <= 0)
		{
			printf("Please enter a number 0 or greater\n");
			return 0;
		}
	}
	else if (argc == 3)
	{
		row = atoi(argv[1]);
		col = atoi(argv[2]);
		
		if (row <= 0 || col <= 0)
		{
			printf(" Please enter numbers 0 or greater");
			return;
		}
	}
	else	// too many commands
	{
		printf("Too many commands entered\n");
		printf("Enter either one or two integers to represent the amount rows and columns respectively\n");
		return;
	}
	
	int seats[row][col];
	
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			seats[i][j] = 0;
		}
	}
	
	remainingSeats = row*col;
	
	int connfd, len;
	struct sockaddr_in servaddr, cli;
	
		
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("Socket creation failed...");
		exit(0);
	}
	else
	{
		printf("Socket successfully created\n");
	}
	
	bzero(&servaddr, sizeof(servaddr));
	
	// assign IP and port
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	
	// Bind new socket to IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0)
	{
		printf("Socket bind failed...\n");
		exit(0);
	}
	else
	{
		printf("Server bind is successful\n");
	}
	
	// server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0)
	{
		printf("Listening failed...\n");
		exit(0);
	}
	else
	{
		printf("Server is now listening\n");
	}
	len = sizeof(cli);
	
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0)
	{
		printf("server failed to accept...\n");
		exit(0);
	}
	else
	{
		printf("Server accepted the client\n");
	}
	
	runServer(connfd, (int *) seats, row, col);
	
	close(sockfd);
	
	return 0;
}


