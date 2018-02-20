/* To represent the jobs, we will use a linked list. Since there is no linked list collection in C
 * we creat one 
 * https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm
 *
 */

int commandNumber = 0;
struct node *head_job = NULL;
struct node *current_job = NULL;


struct node {
	char * arg;
	int number; // the job number
	int pid; // the process id of the a specific process
	struct node *next; // when another process is called you add to the end of the linked list
	struct node *previous;
};


/* Add a job to the list of jobs
	@param args user command
	@param process_id child id
 */
void addToJobList(char *args[], int process_pid) {

	//struct node *
	struct node *job = malloc(sizeof(struct node));

		// add data
		job->arg = args[0];
		commandNumber ++;
		job->number = commandNumber;
		job->pid = process_pid;

		//the new head is also the current node

		// since we are the adding at the beginning
		job->previous = NULL;

		// add the rest of the list at the end of the new job
		job->next = head_job;

		// change previous of head to new job
		if(head_job != NULL)
			head_job->previous = job;

		// move the head pointer to job
		head_job = job;
}

// Print jobs
void print_jobs(){
	int status;
	
	current_job = head_job;
	while(current_job != NULL){
		
		waitpid(current_job->pid, &status, WNOHANG);

		// Job is finished
		if (waitpid(current_job->pid, &status, WNOHANG) ==  -1){
		// printf("%d %d %d \n",WEXITSTATUS (status),WIFSIGNALED (status), WCOREDUMP (status));
		// if(WEXITSTATUS (status) != 0 | WIFSIGNALED (status) != 0 | WCOREDUMP (status) != 0){
			struct node *temp = NULL;
			// Remove job at head
			if (head_job->pid == current_job->pid){
				// move head
				head_job = current_job->next;
				// let previous to NULL
				if (current_job->next != NULL)
					head_job->previous = NULL;
				printf("%d: removing job %s\n",current_job->number, current_job->arg);
				
				// get the current to new head
				temp = current_job;
				current_job = head_job;

				// free up the deleted job
				free(temp);
			}
			// middlw of the list
			else{
				// set the next
				current_job->previous->next = current_job->next;
				if (current_job->next != NULL)
					current_job->next->previous = current_job->previous;
				printf("%d: removing job %s\n",current_job->number, current_job->arg);
				temp = current_job;
				current_job= current_job->next;
				free(temp);
			}

		}
		// Job is not finished
		else{
			printf("%d: job running %s\n",current_job->number, current_job->arg);
			
			//increment current
			current_job = current_job->next;
		}
	}
}


// Send process to the foreground
int setToForground(int number){
	//int status;
	current_job = head_job;

	// Iter through list
	while(current_job != NULL){

		// Process number matches, Remove process from list
		if (current_job->number == number){
			struct node *temp = NULL;//malloc(sizeof(struct node));
			printf("printing %s to forground\n",current_job->arg );
		
			// Remove at head
			if (current_job == head_job){
				// move head
				head_job = current_job->next;
				// let previous to NULL
				if (current_job->next != NULL)
					head_job->previous = NULL;
				printf("%d: moving to forground %s\n",current_job->number, current_job->arg);
				
				// get the current to new head
				temp = current_job;
				current_job = head_job;
				int pid = temp->pid;
				// free up the deleted job
				free(temp);
				return pid;
			}

			// middle of the list
			else{
				// set the next
				current_job->previous->next = current_job->next;
				if (current_job->next != NULL)
					current_job->next->previous = current_job->previous;
				printf("%d: removing job %s\n",current_job->number, current_job->arg);
				temp = current_job;
				int pid = temp->pid;
				current_job= current_job->next;
				free(temp);
				return pid;
			}
		}
		// increment
		else{
						//increment current
			current_job = current_job->next;
		}

	}
	return 0;
}