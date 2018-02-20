#include  <stdio.h>
#include  <sys/types.h>


void  ChildProcess(int x);                /* child process function  */
void  ParentProcess(int x);               /* parent process function */

void  main(void)
{
     int x = 10; // notice how this variable will be copied to both child and parent process 
     pid_t  pid;// this is just a type def for int really.. 

     pid = fork();
     if (pid == 0) // the fork will return 0 to the child process 
          ChildProcess(x);
     else
          ParentProcess(x); // the fork will return the child ID to the parent process
}

void  ChildProcess(int x)
{
     x = x + 2;
     printf("This line is from child, value = %d\n", x);
     printf("*** Child process is done ***\n");
}

void  ParentProcess(int x)
{
     x = x + 3;
     printf("This line is from parent, value = %d\n", x);
     printf("*** parent process is done ***\n");
}