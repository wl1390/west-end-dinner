#include <stdio.h>
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>

#include "macro.h"

int main(int argc, char **argv)
{	
	int id = 0;
	int err = 0;
	int i;
	struct SharedMemory *shm;

	id = shmget(IPC_PRIVATE, 1, 0666|IPC_CREAT); if (id == -1) perror ("Creation");
	
	shm = (struct SharedMemory *) shmat(id, (void*)0, 0);

	initiateSharedMemory(shm);
	printf("Allocated Shared Memory with ID: %d\n",(int)id);
	
	printf("Opening the restaurant\n");
	operateRestaurant(shm, 1);

	//TODO load the menu into shared memory

	printf("Creating cashiers and server\n");
	char id_string[16];
	sprintf(id_string, "%d", id);
	char *cashierargs[] = {"./cashier", "-s", SERVICETIME, "-b", BREAKTIME, "-m", id_string, NULL};
	char *serverargs[] = {"./server", "-m", id_string, NULL};

	if (fork() == 0)
	{	
		printf("starting server...\n");
		execvp(serverargs[0], serverargs);
		exit(1);
	}

	for (i = 0; i < MaxNumOfCashiers; i++)
	{
		if (fork() == 0)
		{
			printf("starting cahsier...\n");
			execvp(cashierargs[0], cashierargs);
			exit(1);
		}
	}

	printf("Closing the restaurant\n");
	operateRestaurant(shm, 0);

	while(wait(NULL) > 0);


	printf("all worker done ...\n");
	

	//TODO collect data  and analyze


	/* Remove segment */
	destroySharedMemory(shm);
	err = shmctl(id, IPC_RMID, 0); if (err == -1) perror ("Removal.");

	return 0;
}
