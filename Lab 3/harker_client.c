
// Matthew Harker
// CS 470
// client side

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define MAX 128
#define DEFAULT_PORT 5457
#define DEFAULT_TIMEOUT 5
#define SA struct sockaddr

typedef struct {
	char* ip;
	int port;
	int timeout;
} Config;


Config readFile()
{
	Config cfg;
	FILE *fp = fopen("config.ini", "r");
	
	// if there is no file
	if(fp == NULL)
	{
		printf("No config file found, using default values\n");
		cfg.ip = "127.0.0.1";
		cfg.port = DEFAULT_PORT;
		cfg.timeout = DEFAULT_TIMEOUT;
		
		return cfg;
	}
	
	char buff[1024];
	char* configFile = fgets(buff, 1024, fp);
	
	if (configFile == NULL)
	{
		perror(NULL);
		fprintf(stderr, "Issue reading the config file", "config.ini");
		exit(0);
	}
	
	if(fclose(fp) != 0)
	{
		perror(NULL);
		fprintf(stderr, "Issue closing the config file", "config.ini");
		exit(0);
	}

	cfg.ip = strtok(configFile, " \n");
	cfg.port = atoi(strtok(NULL, " \n"));
	cfg.timeout = atoi(strtok(NULL, " \n"));
		
	return cfg;
}

void runClient(int sockfd, char runType)
{
	char buff[MAX];
	int n;
	
	if (runType == 'm')
	{
		while(1)
		{
			bzero(buff, sizeof(buff));
			printf("Enter the seat to buy. Format is <row> <col>: ");
			n = 0;
			while ((buff[n++] = getchar()) != '\n')
			{
				;
			}
		
			write(sockfd, buff, sizeof(buff));
			bzero(buff, sizeof(buff));
			read(sockfd, buff, sizeof(buff));
			printf("%s\n", buff);
		
			if ((strncmp(buff, "exit", 4)) == 0)
			{
				printf("client exit\n");
				break;
			}
		}
	}
	else if (runType == 'a')
	{
		bzero(buff, sizeof(buff));
		printf("Buying random seats\n");
		
		char* msg = "random";
		write(sockfd, msg, sizeof(buff));
	}
}

int main(int argc, int* argv[])
{
	// set ip, port, and timeout
	Config cfg = readFile();
	printf("IP: %s\n", cfg.ip);
	printf("Port: %i\n", cfg.port);
	printf("Timeout: %i\n", cfg.timeout);
	
	char buyType;
	
	char* ip = cfg.ip;
	int port = cfg.port;
	int timeout = cfg.timeout;

	
	if (argc == 1)
	{
		printf("Not enough arguments, please include either \"automatic\" or \"manual\"\n");
		return 0;
	}
	else if(argc == 2)
	{
		if (strcmp(argv[1], "automatic\n"))
		{
			buyType = 'a';
		}
		else if (strcmp(argv[1], "manual\n"))
		{
			buyType = 'm';
		}
		else
		{
			printf("Argument not recognized, please use \"automatic\" or \"manual\"\n");
			return 0;
		}
	}
	else
	{
		printf("Too many arguments\n");
		return 0;
	}
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed...\n");
		exit(0);
	}
	else
	{
		printf("Socket successfully created\n");
	}
	
	bzero(&servaddr, sizeof(servaddr));
	
	// assign IP, port
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(port);
	
	// connect the client socket to server socket
	int i = 0;
	printf("Attempting connection...\n");
	while (1)
	{
		if (i >= timeout)
		{
			printf("Connection timed out, terminating program\n");
			exit(0);
		}
		else if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0)
		{
			printf("Retrying connection");
			fflush(stdout);
			sleep(1);
			
			printf(".");
			fflush(stdout);
			sleep(1);
			
			printf(".");
			fflush(stdout);
			sleep(1);
			
			printf(".");
			fflush(stdout);
			sleep(1);
			
			printf("\n");
		}
		else
		{
			printf("Connected to the server\n");
			break;
		}
		
		++i;
	}
	
	runClient(sockfd, buyType);
	
	close(sockfd);
	
	return 0;
}


