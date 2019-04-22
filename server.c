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
		if ((*shm).waiting != 0)
		{
			int temp = rand()%SERVERTIME + 1;
			sleep(temp);
			(*shm).waiting = 0;
		}

	}

	printf("server finishes working...\n");

	return 0;
}