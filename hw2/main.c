#define _SVID_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 500
#define _XOPEN_SORUCE 600
#define _XOPEN_SORUCE 600

#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 20
#define SECTIONA_START 100
#define SECTIONA_SEATS 10
#define SECTIONB_START 200
#define SECTIONB_SEATS 10
#define MAX_NAME_SIZE 10
#define BUFF_SHM "/OS_BUFF"
#define BUFF_MUTEX_A "/OS_MUTEX_A"
#define BUFF_MUTEX_B "/OS_MUTEX_B"

//declaring semaphores names for local usage
sem_t *mutexA;
sem_t *mutexB;

//declaring the shared memory and base address
int shm_fd;
void *base;

//structure for indivdual table
struct table
{
    int num;
    int isINIT;
    char name[MAX_NAME_SIZE];
}shared_memory;

void initTables(struct table *base)
{
    //capture both mutexes using sem_wait
    sem_wait(mutexA);
    sem_wait(mutexB);
    
    //initialise the tables with table numbers
    for (int i = 0; i < SECTIONA_SEATS; i++)
    {   
        // Sextion A
        base[i].isINIT = 1;
        base[i].num = SECTIONA_START + i;
        strcpy(base[i].name,"");
        // Section B
        base[i+SECTIONA_SEATS].num = SECTIONB_START + i;
        strcpy(base[i + SECTIONA_SEATS].name,"");
        //printf("%d %d\n", base[i].num, base[i + SECTIONA_SEATS].num);
    }
    //perform a random sleep  
    sleep(rand() % 10);

    //release the mutexes using sem_post
    sem_post(mutexA);
    sem_post(mutexB);
    return;
}

void printTableInfo(struct table *base)
{
    //capture both mutexes using sem_wait
    sem_wait(mutexA);
    sem_wait(mutexB);
    
    //print the tables with table numbers and name
    printf("\n");
    for(int i = 0; i < BUFF_SIZE; i++)
        printf("Table %d: reserve under: %s\n", base[i].num, base[i].name);
    
    //perform a random sleep  
    sleep(rand() % 10);
    
    //release the mutexes using sem_post
    sem_post(mutexA);
    sem_post(mutexB);
    return; 
}

void reserveSpecificTable(struct table *base, char *nameHld, char *section, int tableNo)
{
    switch (section[0])
    {
    case 'A':
        //capture mutex for section A
        sem_wait(mutexA);
        
        //check if table number belongs to section specified
        //if not: print Invalid table number
        if (SECTIONA_START > tableNo | tableNo >= SECTIONA_START + SECTIONA_SEATS)
            printf("print Invalid table number");
        else
        {
            //reserve table for the name specified
            //if cant reserve (already reserved by someone) : print "Cannot reserve table"
            int index = tableNo - SECTIONA_START;
            if(!strcmp(base[index].name,""))
            {
                strncpy(base[index].name, nameHld, sizeof base[index].name - 1);
                base[index].name[MAX_NAME_SIZE- 1] = '\0';
            }
            else
            {
                printf("Cannot reserve table\n");
            }
        }
        
       // release mutex
        sem_post(mutexA);
        break;
    case 'B':
        //capture mutex for section B
        sem_wait(mutexB);
        
        //check if table number belongs to section specified
        //if not: print Invalid table number 
        
        if (SECTIONB_START > tableNo | tableNo >= SECTIONB_START + SECTIONB_SEATS)
            printf("print Invalid table number");
        else
        {   //reserve table for the name specified
            //if cant reserve (already reserved by someone) : print "Cannot reserve table"
            int index = tableNo - SECTIONB_START + SECTIONB_SEATS;
            if(!strcmp(base[index].name,""))
            {
                strncpy(base[index].name, nameHld, sizeof base[index].name - 1);
                base[index].name[MAX_NAME_SIZE- 1] = '\0';
            }
            else
            {
                printf("Cannot reserve table\n");
            }
        }
        // release mutex
        sem_post(mutexB);
       break;
    }
    return;
}

void reserveSomeTable(struct table *base, char *nameHld, char *section)
{
    int idx = -1;
    int i;
    switch (section[0])
    {
    case 'A':
        //capture mutex for section A
        sem_wait(mutexA);
        int isreservedA = 0;
        for(int i = 0 ; i < SECTIONA_SEATS; i++)
        {
        //look for empty table and reserve it ie copy name to that struct
            if(!strcmp(base[i].name,""))
            {
                strncpy(base[i].name, nameHld, sizeof base[i].name - 1);
                base[i].name[MAX_NAME_SIZE- 1] = '\0';
                isreservedA = 1;
                break;
            }

        }
        //if no empty table print : Cannot find empty table
        if (isreservedA == 0)
        {
            printf("Cannot find empty table\n");
        }

        //release mutex for section A
        sem_post(mutexA);
        break;
    case 'B':
        //capture mutex for section B
        sem_wait(mutexB);
        int isreservedB = 0;
        for(int i = SECTIONA_SEATS ; i < BUFF_SIZE; i++)
        {
        //look for empty table and reserve it ie copy name to that struct
            if(!strcmp(base[i].name,""))
            {
                strncpy(base[i].name, nameHld, sizeof base[i].name - 1);
                base[i].name[MAX_NAME_SIZE- 1] = '\0';
                isreservedB = 1;
                break;
            }

        }
        //if no empty table print : Cannot find empty table
        if (isreservedB == 0)
        {
            printf("Cannot find empty table\n");
        }

        //release mutex for section B
        sem_post(mutexB);
        break;
    }
}

int processCmd(char *cmd, struct table *base)
{
    char *token;
    char *nameHld;
    char *section;
    char *tableChar;
    int tableNo;
    int isINIT = 1;
    token = strtok(cmd, " ");
    // Check if table init
    sem_wait(mutexA);
    sem_wait(mutexB);

    if (!base[0].isINIT)
    {
        isINIT = 0;
    }

    sem_post(mutexA);
    sem_post(mutexB);
    switch (token[0])
    {
    case 'r':
        // input validation check
        if (strcmp(token,"reserve"))
        {   printf("Wrong input\n");
            break;
        }
        // Check if table is initialized
        if (!isINIT)
        {
            printf("Initialize First \n");
            break;
        }
        nameHld = strtok(NULL, " ");
        section = strtok(NULL, " ");
        tableChar = strtok(NULL, " ");
        if (nameHld == NULL || section == NULL)
        {   
            printf("Wrong input\n");
            break;
        }
        if (tableChar != NULL)
        {
            tableNo = atoi(tableChar);
            reserveSpecificTable(base, nameHld, section, tableNo);
        }
        else
        {
            reserveSomeTable(base, nameHld, section);
        }
        sleep(rand() % 10);
        break;
    case 's':
        // Input validation check
        if (strcmp(token,"status\n"))
        {
            printf("Wrong input \n");
            break;
        }
        // Check if the table is inilialized
        if (!isINIT)
        {
            printf("Initialize First \n");
            break;
        }
        printTableInfo(base);
        break;
    case 'i':
        if (!strcmp(token,"init\n") || !strcmp(token,"init"))
        {
            initTables(base);
            break;
        }
        printf("Wrong input \n");
        break;
    case 'e':
        if (!strcmp(token,"exit\n") || !strcmp(token,"exit"))
        {
            return 0;
        }
        printf("Wrong input \n");
        break;
    default :
            printf("Wrong input \n");
            break;
    }
    return 1;
}

int main(int argc, char * argv[])
{
    int fdstdin;
    int file_in;
    // file name specifed then rewire fd 0 to file 
    if(argc>1)
    {
        //store actual stdin before rewiring using dup in fdstdin
        fdstdin = dup(0);
        close(0);
        
        //perform stdin rewiring as done in assign 1
        file_in = open(argv[1], O_RDONLY);
        printf("replacing file descriptor \n");
       
    }
    //open mutex BUFF_MUTEX_A and BUFF_MUTEX_B with inital value 1 using sem_open
    if ((mutexA = sem_open (BUFF_MUTEX_A, O_CREAT, 0660, 1)) == SEM_FAILED)
        printf("sem_open error");
    if ((mutexB = sem_open (BUFF_MUTEX_B, O_CREAT, 0660, 1)) == SEM_FAILED)
        printf("sem_open error");

    //opening the shared memory buffer ie BUFF_SHM using shm open
    shm_fd = shm_open (BUFF_SHM, O_RDWR | O_CREAT, 0660);
    if (shm_fd == -1)
    {
        printf("prod: Shared memory failed: %s\n", strerror(errno));
        exit(1);
    }

    //configuring the size of the shared memory to sizeof(struct table) * BUFF_SIZE usinf ftruncate
    if (ftruncate (shm_fd, sizeof (shared_memory) * BUFF_SIZE) == -1)
       printf("ftruncate error");

    //map this shared memory to kernel space
    base = mmap(NULL, sizeof (shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (base == MAP_FAILED)
    {
        printf("prod: Map failed: %s\n", strerror(errno));
        // close and shm_unlink?
        shm_unlink(BUFF_SHM);
        exit(1);
    }

    //intialising random number generator
    time_t now;
    srand((unsigned int)(time(&now)));

    //array in which the user command is held
    char cmd[100];
    int cmdType;
    int ret = 1;
    while (ret)
    {
        printf("\n>>");
        if (fgets(cmd, 100 , stdin) == NULL)
        {
            dup2(fdstdin, 0);
            argc = 1;
            //close(file_in);
        }
        //gets(cmd);
        if(argc>1)
        {
            printf("Executing Command : %s\n",cmd);
        }
        ret = processCmd(cmd, base);
    }
    
    //close the semphores
    sem_unlink(BUFF_MUTEX_A);
    sem_unlink(BUFF_MUTEX_B);
    sem_close(mutexA);
    sem_close(mutexB);
    

    //reset the standard input
    if(argc>1)
    {
        //using dup2
        dup2(fdstdin, 0);
        close(file_in);
    }

    //unmap the shared memory
    if (munmap (base, sizeof (shared_memory) * BUFF_SIZE) == -1)
        printf ("munmap");
    close(shm_fd);
    return 0;
}