
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

//Initiliaze the args(input) to null once the command has been processed
// this is to clear it to accept another command in the next while loop
void initialize(char *args[]);


int getcmd(char *line, char *args[], int *background)
{
	int i = 0;
	char *token, *loc;

	//Copy the line to a new char array because after the tokenization a big part of the line gets deleted since the null pointer is moved
	char *copyCmd = malloc(sizeof(char) * sizeof(line) * strlen(line));
	sprintf(copyCmd, "%s", line);

	// Check if background is specified..
	if ((loc = index(line, '&')) != NULL) {
		*background = 1;
		*loc = ' ';
	} else
		*background = 0;

	//Create a new line pointer to solve the problem of memory leaking created by strsep() and getline() when making line = NULL
	char *lineCopy = line;
	while ((token = strsep(&lineCopy, " \t\n")) != NULL) {
		for (int j = 0; j < strlen(token); j++)
			if (token[j] <= 32)
				token[j] = '\0';
		if (strlen(token) > 0)
			args[i++] = token;
	}

	return i;
}
void initialize(char *args[]) {
	for (int i = 0; i < 20; i++) {
		args[i] = NULL;
	}
	return;
}

int main(void) {
	char *args[20];
	int bg;

	char *user = getenv("USER");
	if (user == NULL) user = "User";

	char str[sizeof(char)*strlen(user) + 4];
	sprintf(str, "\n%s>> ", user);

	while (1) {
		initialize(args);
		bg = 0;

		int length = 0;
		char *line = NULL;
		size_t linecap = 0; // 16 bit unsigned integer
		sprintf(str, "\n%s>> ", user);
		printf("%s", str);

		/*
		Reads an entire line from stream, storing the address of
		the buffer containing the text into *lineptr.  The buffer is null-
		terminated and includes the newline character, if one was found.
		check the linux manual for more info
		*/


		length = getline(&line, &linecap, stdin);
		if (length <= 0) { //if argument is empty
			exit(-1);
		}
		int cnt  = getcmd(line, args, &bg);
		if (!strcmp("ls", args[0])) { // returns 0 if they are equal , then we negate to make the if statment true


			printf("You're trying to call ls \n");

			pid_t  pid;// this is just a type def for int really..
			pid = fork();
			if (pid == 0) 
				execvp(args[0], args);
			else {
				int status;
				waitpid(pid, &status, WUNTRACED);
			}

		}
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
		free(line);
	}


}
