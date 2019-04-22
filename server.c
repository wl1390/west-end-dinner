#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "macro.h"

int main(int argc, char **argv)
{
	int err;
	struct SharedMemory *shm;
	int client;
	int order;
	int shmid;

	getSharedMemory(&shmid);
	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);

	printf("server starts working...\n");

	while((*shm).open == 1)
	{
		if ((*shm).count != 0)
		{
			sem_wait(&(*shm).sp5);
			client = (*shm).waiting_clients[(*shm).start];
			order = (*shm).orders[(*shm).start];
			(*shm).count--;
			(*shm).start = ((*shm).start + 1) % MaxNumOfClients;
			sem_post(&(*shm).sp5);

			int temp = 5;

			//shoudl be according to the menu
			sleep(temp);
			printf("food for client %d is ready.\n", client);
			(*shm).ready = client;
		}	
	}

	printf("server finishes working...\n");

	return 0;
}