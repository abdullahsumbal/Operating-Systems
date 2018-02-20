/* To represent the jobs, we will use a linked list. Since there is no linked list collection in C
 * we creat one 
 * https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm
 *
 */

int commandNumber = 0;
struct node *head_job = NULL;
struct node *current_job = NULL;



struct node {
	int number; // the job number
	int pid; // the process id of the a specific process
	struct node *next; // when another process is called you add to the end of the linked list
};

/* Add a job to the list of jobs
 */
void addToJobList(char *args[]) {

	struct node *job = malloc(sizeof(struct node));

	//If the job list is empty, create a new head
	if (head_job == NULL) {
		job->number = 1;
		job->pid = process_pid;

		//the new head is also the current node
		job->next = NULL;
		head_job = job;
		current_job = head_job;
	}

	//Otherwise create a new job node and link the current node to it
	else {

		job->number = current_job->number + 1;
		job->pid = process_pid;

		current_job->next = job;
		current_job = job;
		job->next = NULL;
	}
}