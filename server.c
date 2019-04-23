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
	struct SharedMemory *shm;
	int shmid;

	int client, order;
	
	/* Attaching shared memory */
	getSharedMemory(&shmid);
	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);

	while((*shm).restaurant_is_open == 1)
	{	
		/* Block for Tserver time is there is client waiting */
		if ((*shm).server_busy != 0)
		{	
			int temp = rand()%SERVERTIME + 1;
			printf("Server serving: %d seconds ...\n", temp);
			sleep(temp);
			(*shm).server_busy = 0;
		}

	}

	/* Detaching shared memory */
	if (shmdt((void *)shm) == -1) perror ("Detachment.");

	return 0;
}