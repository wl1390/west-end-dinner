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
	FILE *fp;

	id = shmget(IPC_PRIVATE, 1, 0666|IPC_CREAT); if (id == -1) perror ("Creation");
	
	shm = (struct SharedMemory *) shmat(id, (void*)0, 0);

	initiateSharedMemory(shm);
	
	fp = fopen("memoryId", "w");
	fprintf(fp, "%d",id);
	fclose(fp);

	printf("Allocated Shared Memory with ID: %d\n",(int)id);

	printf("Opening the restaurant\n");

	sem_wait(&(*shm).sp2);
	(*shm).open = 1;
	sem_post(&(*shm).sp2);

	//TODO load the menu into shared memory

	printf("Creating cashiers and server\n");

	if (fork() == 0)
	{	

		printf("starting server...\n");
		char *serverargs[] = {"./server", NULL};
		execvp(serverargs[0], serverargs);
		exit(1);
	}

	for (i = 0; i < MaxNumOfCashiers; i++)
	{
		if (fork() == 0)
		{
			printf("starting cahsier %d...\n", i);
			char cahsierNumber[16];
			char serviceTime[16];
			char breakTime[16];
			sprintf(cahsierNumber, "%d", i);
			sprintf(serviceTime, "%d", SERVICETIME);
			sprintf(breakTime, "%d", BREAKTIME);
			char *cashierargs[] = {"./cashier", "-i", cahsierNumber,"-s", serviceTime, "-b", breakTime, NULL};
			execvp(cashierargs[0], cashierargs);
			exit(1);
		}
	}

	getchar();

	printf("Closing the restaurant\n");
	sem_wait(&(*shm).sp2);
	(*shm).open = 0;
	sem_post(&(*shm).sp2);

	while(wait(NULL) > 0);


	printf("all worker done ...\n");
	

	//TODO collect data  and analyze


	/* Remove segment */
	destroySharedMemory(shm);
	err = shmctl(id, IPC_RMID, 0); if (err == -1) perror ("Removal.");

	return 0;
}
