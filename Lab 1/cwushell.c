#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#define clear() printf("\033[H\033[J") 

char prompt[] = "cwushell>";

void init()
{
    clear();
    printf("***************************************");
    printf("\n***************************************");
    printf("\n************** CWU SHELL **************");
    printf("\n*********** by Matt Harker ************");
    printf("\n***************************************");
    printf("\n***************************************");
    
    printf("\n\nFor a list of commands enter \"manual\"\n\n");
    
}

// displays a list of commands
void displayHelp()
{
    printf("\n ******** HELP ********");
    printf("\nList of commands:");
    
    printf("\nprompt <new_prompt>");
    printf("\n    modifies the current shell prompt to <new_prompt>");
    
    printf("\ncpuinfo -switch");
    printf("\n    available switches: -c, -t, -n");
    
    printf("\nmeminfo -switch");
    printf("\n    availabe switches: -t, -u, -c");
    
    printf("\nmanual");
    printf("\n    displays a list of commands available in this shell");
    
    printf("\nFor more info on specific commands, use -h or -help as a swicth\n");
}

// shows a help message for the prompt command
void displayPromptHelp()
{
    printf("In order to use the prompt command, enter it as follows: \n");
    printf("    prompt <new_prompt>\n");
    printf("Enter what you want the new prompt to be where it says <new_prompt>\n");    
}

// changes the shell's prompt
void changePrompt(char* str)
{
    strcpy(prompt, str);
}

// displays a help message for the cpuinfo command
void displayCpuHelp()
{
    printf("cpuinfo has the following switches:\n");
    printf("    -c: displays the clock speed\n");
    printf("    -t: displays the cpu manufacturer\n");
    printf("    -n: displays the number of cores\n");
}

// displays the speed of the cpu clock
void displayCpuClock()
{
    system("lscpu | grep 'MHz'");
}

// displays the brand of the cpu
void displayCpuType()
{
    system("cat /proc/cpuinfo | grep 'vendor' | uniq");
}

// prints the number of cores
void displayCpuCores()
{
    // write is used as system() is not buffered
    write(1, "Number of cores: ", 17);  
    system("nproc --all");
}


// displays a help message for the meminfo command
void displayMemHelp()
{
    printf("meminfo has the following switches:\n");
    printf("    -t: displays the amount of available ram\n");
    printf("    -u: displays the amount of used ram\n");
    printf("    -c: displays the size of the L2 cache\n");
}

// displays the amount of available ram
void displayAvailableRam()
{
    system("cat /proc/meminfo | grep 'MemAvailable'");
}

// displays the amount of used ram
void displayUsedRam()
{
    system("cat /proc/meminfo | grep 'Active:'");
}

// displays the size of the L2 chache/core in bytes
void displayCacheSize()
{
    write(1, "Size of L2 cache (bytes): ", 26);
    long memFree = system("getconf LEVEL2_CACHE_SIZE");
}

int startsWith(const char* pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

int endsWithAngle(const char* str)
{
    str = strrchr(str, '>');
    
    if(str != NULL)
        return (strcmp(str, ">"));
    
    return(-1);
}

int main(int argc, char** argv)
{
    init();
    
    char inputString[100];   // the input to be executed

    while (1)
    {
        printf("\n%s ", prompt);
        fgets(inputString, sizeof(inputString), stdin);
        
        // displays the general help message
        if (strcmp(inputString, "manual\n") == 0 || strcmp(inputString, "help\n") == 0)
        {
            displayHelp();
        }
        
        
        // cpu info
        else if(strcmp(inputString, "cpuinfo -c\n") == 0)
        {
            displayCpuClock();
        }
        else if(strcmp(inputString, "cpuinfo -t\n") == 0)
        {
            displayCpuType();
        }
        else if(strcmp(inputString, "cpuinfo -n\n") == 0)
        {
            displayCpuCores();
        }
        else if(strcmp(inputString, "cpuinfo\n")  == 0 || strcmp(inputString, "cpuinfo -h\n") == 0
                                          || strcmp(inputString, "cpuinfo -help\n") == 0)
        {
            displayCpuHelp();
        }
          
            
        // meminfo
        else if(strcmp(inputString, "meminfo\n") == 0 || strcmp(inputString, "meminfo -h\n") == 0
                                               || strcmp(inputString, "meminfo -help\n") == 0)
        {
            displayMemHelp();
        }
        else if (strcmp(inputString, "meminfo -t\n") == 0)
        {
            displayAvailableRam();
        }    
        else if (strcmp(inputString, "meminfo -u\n") == 0)
        {
            displayUsedRam();
        }
        else if(strcmp(inputString, "meminfo -c\n") == 0)
        {
            displayCacheSize();
        }
        
        
        // The Prompt command
        else if (strcmp(inputString, "prompt\n") != 0 && strcmp(inputString, "prompt -h\n") != 0
                                                      && strcmp(inputString, "prompt -help\n") != 0
                                                      && startsWith("prompt", inputString))
        {
            char* token;
            token = strtok(inputString, " \n");
            token = strtok(NULL, " \n");
            
            strcpy(prompt, token);
            
        }
        else if (strcmp(inputString, "prompt\n") == 0 || strcmp(inputString, "prompt -h\n") == 0
                                                   || strcmp(inputString, "prompt -help\n") == 0)
        {
            displayPromptHelp();
        }
        
        // the exit command
        else if (strcmp(inputString, "exit\n") != 0 && startsWith("exit", inputString))
        {
            char* token;
            
            token = strtok(inputString, " \n");
            token = strtok(NULL, " \n");
            
            if (isdigit(token) == 1)
            {
                int extCode = atoi(token);
                return extCode;
            }
            else
                return 0;
            
        }
        else if (strcmp(inputString, "exit\n") == 0)
        {
        
            return 0;
        }
        
        
        
        // to handle anything else
        else
        {
        
            system(inputString);
        }
    }
}


