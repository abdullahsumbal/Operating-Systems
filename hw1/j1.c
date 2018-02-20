./..:
hw1
/tmp:
atom-1.17.2-kyilma.sock
Atom Crashes
config-err-6Em8eX
config-err-cZ498i
config-err-DR5nBd
config-err-eDLG6f
config-err-EFpbs3
config-err-EKWuFR
config-err-jAXIp2
config-err-rDvNr0
config-err-u3Zwb0
drjava-junit9066571397480092071.jar
firefox_agodar1
firefox_david.ivanov
firefox_msumba
gnome-software-FO9D7Y
hsperfdata_agodar1
hsperfdata_david.ivanov
hsperfdata_kyilma
hsperfdata_oasad
hsperfdata_rrazaf1
jarfscacheagodar1
jna--1052066875
krb5cc_1357348_uueg1t
kwallet5_david.ivanov.socket
kwallet5_evan.roach.socket
kwallet_david.ivanov.socket
kwallet_evan.roach.socket
mozilla_agodar10
orbit-msumba
pip-2IHfLI-uninstall
problems-index8401584832044690483.zip
systemd-private-d5a7deb2523b4b8e88372f3a84bc28df-colord.service-FjkYX9
systemd-private-d5a7deb2523b4b8e88372f3a84bc28df-rtkit-daemon.service-2hh2Kn
systemd-private-d5a7deb2523b4b8e88372f3a84bc28df-systemd-timesyncd.service-v3lVux
tracker-extract-files.1357348
tracker-extract-files.23985
tracker-extract-files.26035
unity_support_test.0
VMwareDnD
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include "jobs.c"

//
// This code is given for illustration purposes. You need not include or follow this
// strictly. Feel free to writer better or bug free code. This example code block does not
// worry about deallocating memory. You need to ensure memory is allocated and deallocated
// properly so that your shell works without leaking memory.
//

/**
	Gets the command from the interface
	@param prompt initial prompt
	@param args array of command split by space
	@param background indicator for & sign in command

	@return length of args + 1
*/

int child_pid;
int isChildProcess = 0;

int getcmd(char *prompt, char *args[], int *background, int *redirect)
{
	int length, i = 0;
	char *token, *loc;
	char *line = NULL;
	size_t linecap = 0;
	printf("\n%s",prompt);
	length = getline(&line, &linecap, stdin);
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
	if ((loc = index(line, '>')) != NULL) {
		*redirect = 1;
	} else
		*redirect = 0;
	while ((token = strsep(&line, " \t\n")) != NULL) {
		for (int j = 0; j < strlen(token); j++)
			if (token[j] <= 32)
				token[j] = '\0';
		if (strlen(token) > 0)
			args[i++] = token;
	}
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
{
	if(child_pid){
		printf("\nSignal caught and kill the pid %d!!\n", child_pid);
		kill(child_pid, SIGKILL);
	}
	else{
		printf("\nSignal caught and no process to kill!!\n");
	}
}

// Signal handler for SIGTSTP
static void sigtstpHandler(int sig)
{
	printf("Signal caught and do nothing!!1\n");
}

/**
	Handle file descriptor
	@param args command from user
	@length length of the command
	@return error if -1

*/
int handleRedirect(char * args[], int length){
	int index = 0;
	char *sub_args[20];
	char * file_name;
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
		return -1;
	}
	pid_t pid = fork();
	if (pid == 0){
		int in = open(file_name, O_APPEND | O_WRONLY);
		if(dup2(in, 1) < 0)    return -1;	
		// close unused file descriptors
		execvp(sub_args[0], sub_args);
		close(in);		
	}
	// implement background
	else{

	}
	return 1;
}


int main(void)
{
	char *args[20];
	int backGround;
	int redirect;
	int *status;

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
		backGround = 0;
		redirect = 0;
		initialize(args);
		int cnt = getcmd("\n>> ", args, &backGround, &redirect);

		if (*args){

			// Command jobs
			if (!strcmp("jobs", args[0])) {
				//print_jobs();
			}
			// Command ls
			else if (!strcmp("ls", args[0])) { 
				pid_t  pid;
				pid = fork();
				if (pid == 0) 
					
					if(redirect){
						handleRedirect(args, cnt);
					}
					else
						execvp(args[0], args);
				else {
					if (backGround){
						 waitpid(pid, status, WUNTRACED);
						 addToJobList(args, pid);
						 //print_jobs();
					}
				}
			}
			// Command exit
			else if (!strcmp("exit", args[0])) {
				exit(-1);
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
				//Otherwise go to specified directory
				else {
					result = chdir(args[1]);
				}
				if (result == -1) fprintf(stderr, "cd: %s: No such file or directory", args[1]);
			}
			// Command cat
			else if (!strcmp("cat", args[0])) { // returns 0 if they are equal , then we negate to make the if statment true
				printf("You're trying to call cat \n");

				if (redirect){
					handleRedirect(args, cnt);
				}
				else{
					pid_t  pid;// this is just a type def for int really..
					pid = fork();
					if (pid == 0) 
						execvp(args[0], args);
					else {
						if (backGround){
							 waitpid(pid, status, WUNTRACED);
							 addToJobList(args, pid);
							 //print_jobs();
						}
					}
				}
			}
			// Other commands
			else{

				// other commands
				pid_t  pid;
				pid = fork();
				if (pid == 0){
					execvp(args[0], args);
				}
				else
					child_pid = pid;
			}
			// Set child_pid to 0
			child_pid = 0;
		}
		
		/* the steps can be..:
		(1) fork a child process using fork()
		(2) the child process will invoke execvp()
		(3) if background is not specified, the parent will wait,
		 otherwise parent starts the next command... */
	}
}