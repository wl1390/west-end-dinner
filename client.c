#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "macro.h"


int load_client_argv(int argc, char **argv, int *itemId, int *eatTime)
{
	int i;
	int j;
	int t = 0;
	int e = 0;

	if ((argc != 5)) return 0;

	for (i = 2; i < 5; i+=2)
	{
		for (j = 0; j < strlen(argv[i]); j++)
		{
			if (!isdigit(argv[i][j])) return 0;
		} 
	}
	
	for (i = 1; i < 4; i+=2)
	{
		if (!strcmp(argv[i], "-i"))
		{
			t = 1;
			*itemId = atoi(argv[i+1]);
		}
		else if (!strcmp(argv[i], "-e"))
		{
			e = 1;
			*eatTime = atoi(argv[i+1]);
		}

	}

	if ( (t+e) != 2)	return 0;

	return 1;
}

int main(int argc, char **argv)
{
	int itemId;
	int eatTime;
	pid_t pid;
	int err;
	int cashier;
	struct SharedMemory *shm;
	int shmid;
	
	if (!load_client_argv(argc, argv, &itemId, &eatTime))
	{
		printf("ERROR: client initiation should follow the below format:\n");
		printf("\t./client -i itemId -e eatTime\n");
		return 0;
	}

	pid = getpid();

	getSharedMemory(&shmid);
	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);


	if (itemId < 1 || itemId > (*shm).menu.count) 
	{
		printf("Item %d is not on the menu. Client leaving...\n", itemId);
		return 0;
	}

	if (clientEnter(shm, pid) == 0)
	{
		printf("Restaurant full. Client [%d] leaving...\n", pid);
		return 0;
	}

	printf("client %d enters...\n", pid);

	cashier = order(shm, pid, itemId);

	while((*shm).ordering[cashier] != 0) continue;
	sem_post(&(*shm).sp4);

	printf("client finishes ordering\n");

	srand(pid);
	
	int temp = rand() % ((*shm).menu.maxTime[itemId] - (*shm).menu.minTime[itemId]) + (*shm).menu.minTime[itemId];


	printf("client waiting for %d second for food to be ready...\n", temp);
	sleep(temp);

	printf("food ready. waiting for server now...\n");

	sem_wait(&(*shm).sp6);
	(*shm).waiting = 1;
	printf("server is servicing...\n");
	while((*shm).waiting != 0) continue;
	sem_post(&(*shm).sp6);

	srand(pid);
	temp = rand()%eatTime + 1;

	printf("food ready, eat.");

	sleep(temp);

	/* Detach segment */
	clientLeave(shm, pid);
	printf("client %d leaves...\n", pid);

	err = shmdt((void *)shm); if (err == -1) perror ("Detachment.");
	return 0;
}
