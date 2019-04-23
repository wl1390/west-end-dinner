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
	
	getSharedMemory(&shmid);
	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);

	while((*shm).open == 1)
	{
		if ((*shm).waiting != 0)
		{	
			int temp = rand()%SERVERTIME + 1;
			printf("Server serving: %d seconds ...\n", temp);
			sleep(temp);
			(*shm).waiting = 0;
		}

	}

	if (shmdt((void *)shm) == -1) perror ("Detachment.");

	return 0;
}