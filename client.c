#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <sys/shm.h>

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

	pid_t pid;

	int err;
	struct ClientInfo *clients;


	if (!load_client_argv(argc, argv, &itemId, &eatTime, &shmid))
	{
		printf("ERROR: client initiation should follow the below format:\n");
		printf("\t./client -i itemId -e eatTime -m shmid\n");
		
		return 0;
	}

	pid = getpid();


	/* Attach the segment */
	clients = (struct ClientInfo *) shmat(shmid, (void*) 0,SHM_RDONLY);
	// if (*(int *) AcceptClients == -1) perror("Attachment.");
	// else printf(">> Attached Shared Segment whose Mem content is: %d\n",*AcceptClients);


	if ((*clients).AcceptClients == 0)
	{
		printf("Restaurant is not open. Client [%d] is leaving...\n", pid);
	}
	else if ((*clients).AcceptClients == 1)
	{
		//give pid to the list
		printf("good\n");
	}


	/* Detach segment */
	err = shmdt((void *)clients);
	// if (err == -1) perror ("Detachment.");
	// else printf(">> Detachment of Shared Segment %d\n", err);
	return 0;




	//if restaurant not accepting or not open, leave. 

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