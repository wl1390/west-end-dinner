#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "macro.h"


int load_client_argv(int argc, char **argv, int *itemId, int *eatTime, int *shmid)
{
	int i;
	int j;
	int t = 0;
	int e = 0;
	int m = 0;

	if ((argc != 7)) return 0;

	for (i = 2; i < 7; i+=2)
	{
		for (j = 0; j < strlen(argv[i]); j++)
		{
			if (!isdigit(argv[i][j])) return 0;
		} 
	}
	
	for (i = 1; i < 6; i+=2)
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
		else if (!strcmp(argv[i], "-m"))
		{
			m = 1;
			*shmid = atoi(argv[i+1]);
		}

	}

	if ( (t+e+m) != 3)	return 0;

	return 1;
}


int main(int argc, char **argv)
{
	int itemId;
	int eatTime;
	int shmid;
	int pid;
	int err;
	struct SharedMemory *shm;
	

	if (!load_client_argv(argc, argv, &itemId, &eatTime, &shmid))
	{
		printf("ERROR: client initiation should follow the below format:\n");
		printf("\t./client -i itemId -e eatTime -m shmid\n");
		
		return 0;
	}

	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);

	pid  = 5;
	struct Client client = {pid, itemId, 0};
	
	printf("%d\n", client.pid);
	printf("%d\n", client.itemId);
	printf("%d\n", client.status);


	addClient(&client, shm);
	getchar();
	
	/* Detach segment */
	err = shmdt((void *)shm); if (err == -1) perror ("Detachment.");

	//arrive and check MaxPeople. If full, leave terminate.

	//add to cashier desk
	//wait for available cashier
	//order food and pay to cashier

	//add to service desk
	//wait to get food from server
	//get food from server

	//add to dining desk
	//eat the food 

	//terminate
	return 0;
}