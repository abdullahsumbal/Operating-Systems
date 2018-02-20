#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "job.c"
#include <time.h>

//
// This code is given for illustration purposes. You need not include or follow this
// strictly. Feel free to writer better or bug free code. This example code block does not
// worry about deallocating memory. You need to ensure memory is allocated and deallocated
// properly so that your shell works without leaking memory.
//

int child_pid ;

/**
	Gets the command from the interface
	@param prompt initial prompt
	@param args array of command split by space
	@param background indicator for & sign in command

	@return length of args + 1
*/
int getcmd(char *prompt, char *args[], int *background, int *redirect)
{
	int length, i = 0;
	char *token, *loc;
	char *line = NULL;
	size_t linecap = 0;
	printf("\n%s",prompt);
	length = getline(&line, &linecap, stdin);
	//child_pid = 0;
	if (length <= 0) {
		exit(-1);
	}
	// Check if background is specified..
	if ((loc = index(line, '&')) != NULL) {
		*background = 1;
		*loc = ' ';
	} else
		*background = 0;
	// Check if redirect is specified..
	if ((/*loc = */index(line, '>')) != NULL) {
		*redirect = 1;
	} else
		*redirect = 0;

	//char * line_copy = line;
	while ((token = strsep(&line, " \t\n")) != NULL) {
		for (int j = 0; j < strlen(token); j++)
			if (token[j] <= 32)
				token[j] = '\0';
		if (strlen(token) > 0)
			args[i++] = token;
	}
	//free(line_copy);
	return i;
}

/**
	Initializes the args arary
	@param args array of command separated by space
*/
void initialize(char *args[]) {
	for (int i = 0; i < 20; i++) {
		args[i] = NULL;
	}
}

// Signal handler for SIGIN
static void sigintHandler(int sig)
{	int status;
	//printf("child pid %d\n",child_pid );
	waitpid(child_pid, &status, WNOHANG);
	if((waitpid(child_pid, &status, WNOHANG) !=  -1) & (child_pid != 0)){
		printf("\nSignal caught and kill the pid %d!!\n", child_pid);
		kill(child_pid, SIGKILL);
		child_pid = 0;
	}
	else{
		printf("\n>> ");
	}
}

// Signal handler for SIGTSTP
static void sigtstpHandler(int sig)
{
	printf("\n>> ");
}


/**
	deals with background task
	@param args command from user
	@param background bool
	@pid pid of the child process
*/
void handleBackground(char * args[], int background, pid_t pid){
	// assign child pid for signal handlers
	
	int *status;
	// run process in background
	if (background){
		//waitpid(pid,status,0);
		addToJobList(args, pid);
	}
	// wait for child to finish
	else{
		child_pid = pid;
		waitpid(pid, status, WUNTRACED);
	}
}

void execute(int background, char * args[]){
	
	if (background){
		int w,rem;
		w = rand() % 10;
		rem=sleep(w);
		//handles interruption by signal
		while(rem!=0)
		 {
		 rem=sleep(rem);
		 }
	}
	execvp(args[0], args);	
}

/**
	Handle file descriptor
	@param args command from user
	@length length of the command
	@return error if -1

*/
void handleRedirect(char * args[], int length, int background){
	int index = 0;
	char * file_name;
	char *sub_args[20];
	initialize(sub_args);
	// Find index of redirector symbol
	while(strcmp(">", args[index]) != 0){
		sub_args[index] = args[index];
		index++;
	}
	// File name
	if(args[index++] != NULL){
		file_name = args[index];
	}
	else{
		printf("No file found\n");
	}

	// redirecting 1 an 0 of file descriptor
	int in = open(file_name, O_WRONLY | O_CREAT, 0777);
	if(dup2(in, 1) < 0)    printf("Unable to dup\n");;

	execute(background, sub_args);

}


int main(void)
{
	//int *status = 0;
	int redirect;
	char *args[20];
	int background;

	// Random time handler
	time_t now;
	srand((unsigned int) (time(&now)));

	// signal handling
	if (signal(SIGINT, sigintHandler) == SIG_ERR)
	{
		printf("Error! Could not bind the SIGINT signal handler\n");
		exit(1);
	}

	if (signal(SIGTSTP, sigtstpHandler) == SIG_ERR)
	{
		printf("Error! Could not bind the SIGTSTP signal handler\n");
		exit(1);
	}
	while(1) {
		background = 0;
		redirect = 0;
		initialize(args);
		int length = getcmd("\n>> ", args, &background, &redirect);

		if (*args){

			if(!strcmp("fg", args[0])){
				if(length == 2)
					child_pid = setToForground(atoi(args[1]));
			}
			// Command jobs
			else if (!strcmp("jobs", args[0])) {
				print_jobs();
			}
			// Command ls
			else if (!strcmp("ls", args[0])) {
				pid_t  pid = fork();
				// Child process
				if (pid == 0){

					// handle redirect
					if(redirect)
						handleRedirect(args, length, background);
					else
						execute(background, args);
				}// Parent process
				else{
					handleBackground(args, background, pid);
				}
			}
			// Command exit
			else if (!strcmp("exit", args[0])) {
				exit(EXIT_SUCCESS);
			}
			// Command cd 
			else if (!strcmp("cd", args[0])) {
				int result = 0;
				if (args[1] == NULL) { // this will fetch home directory if you just input "cd" with no arguments
					char *home = getenv("HOME");
					if (home != NULL) {
						result = chdir(home);
					}
					else {
						printf("cd: No $HOME variable declared in the environment");
					}
				}
				// Otherwise go to specified directory
				else {
					result = chdir(args[1]);
				}
				if (result == -1) fprintf(stderr, "cd: %s: No such file or directory", args[1]);
			}
			// Command cat
			else if (!strcmp("cat", args[0])) { // returns 0 if they are equal , then we negate to make the if statment true
				//printf("You're trying to call cat \n");

				// if (redirect)
				// 	handleRedirect(args, length, background);
				//else{
					pid_t pid;
					pid = fork();
					// Child process
					if (pid == 0){
						// handle redirect
						if(redirect)
							handleRedirect(args, length, background);
						else
							execute(background, args);
					}
					else {
						handleBackground(args, background, pid);
					}
				//}
			}
			// Other commands
			else{
				pid_t pid = fork();
				// Child process
				if (pid == 0){
					// handle redirect
					if(redirect)
						handleRedirect(args, length, background);
					else
						execute(background, args);
				}
				else {
					handleBackground(args, background, pid);
				}
			}
		}
		
		/* the steps can be..:
		(1) fork a child process using fork()
		(2) the child process will invoke execvp()
		(3) if background is not specified, the parent will wait,
		 otherwise parent starts the next command... */
	}
}