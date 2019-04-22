#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "macro.h"

int load_server_argv(int argc, char **argv, int *shmid)
{
	int i;

	if ((argc != 3) || (strcmp(argv[1], "-m"))) return 0;
	
	for (i = 0; i < strlen(argv[2]); i++)
	{
		if (!isdigit(argv[2][i])) return 0;
	} 

	*shmid = atoi(argv[2]);

	return 1;
}


int main(int argc, char **argv)
{
	int shmid;
	int err;
	struct SharedMemory *shm;

	pid_t pid;

	if (!load_server_argv(argc, argv, &shmid))
	{
		printf("ERROR: server initiation should follow the below format:\n");
		printf("\t./server -m shmid\n");
		
		return 0;
	}

	pid = getpid();

	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);

	printf("server %d starts working...\n", pid);

	




	//wait for order from cashier
	//give food to client





	printf("server %d finishesworking...\n", pid);

	return 0;
}