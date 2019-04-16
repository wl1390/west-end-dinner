#include <stdio.h>
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>

#include "macro.h"

int main(int argc, char **argv)
{	
	int id = 0;
	int err = 0;
	struct ClientInfo *clients;
	// int *AcceptClients;

	id = shmget(IPC_PRIVATE, 1, 0666|IPC_CREAT);

	// if (id == -1) 
	// 	perror ("Creation");
	// else 	
		printf("Allocated Shared Memory with ID: %d\n",(int)id);

	// AcceptClients = (int *) shmat(id, (void*)0, 0);
	clients = (struct ClientInfo *) shmat(id, (void*)0, 0);

	// if ( *(int *) AcceptClients == -1) 
	// 	perror("Attachment.");
	// else 
	// 	printf("Just Attached Shared Memory whose content is: %d\n",*AcceptClients);

	(*clients).AcceptClients=1;
	// printf("Just Altered Shared Memory content to: %d\n",*AcceptClients);

	printf("Start other process. >"); getchar();

	// /* Print out new value */
	// printf("Content of Shared Mem is now: %d\n", *mem);

	/* Remove segment */
	err = shmctl(id, IPC_RMID, 0);
	// if (err == -1) 
	// 	perror ("Removal.");
	// else 
	// 	printf("Just Removed Shared Segment. %d\n", (int)(err));
	



	//open the restaurant 
	//create shared memory
	//load the menu into shared memory
	//load server and cashiers processes
	//start acccepting clients and start counter
	//while counter on
		//monitor client lists

	//stop accepting new clients and wait for all client process to finish
	//close the restaurant
	//collect data 
	//wait for all cashier and server
	//release sys resource
	//exit

	return 0;
}