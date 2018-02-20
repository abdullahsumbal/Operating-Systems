/**
 * Simple program demonstrating shared memory in POSIX systems.
 *
 * This is the producer process that writes to the shared memory region.
 *
 * Figure 3.17
 *
 * @author Silberschatz, Galvin, and Gagne
 * Operating System Concepts  - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>


int getcmd(char *prompt, char *args[])
{
	int i = 0;
	char *token;
	char *line = NULL;
	size_t linecap = 0;
	printf("\n%s",prompt);
	getline(&line, &linecap, stdin);
	char * line2 = line;
	while ((token = strsep(&line2, " \t\n")) != NULL) {
		for (int j = 0; j < strlen(token); j++)
			if (token[j] <= 32)
				token[j] = '\0';
		if (strlen(token) > 0)
			args[i++] = token;
	}
	free(line);
	free(token);
	return i;
}

int verify_arguments(char* args, char* name, char * section, char* table)
{
	return 1;
}

void initialize(char *args[]) {
	for (int i = 0; i < 20; i++) {
		args[i] = NULL;
	}
}
int main()
{

	char *args[100];
	initialize(args);
	int length = 0;
	while(1) {
		length = getcmd("\n>> ", args);

		if (length > 0)
		{
			printf("first %s\n", args[0] );
			if (strcmp("exit", args[0]) == 0) {
				exit(EXIT_SUCCESS);
			}
		}


	}
	return 0;
}