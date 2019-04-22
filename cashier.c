#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "macro.h"

int load_cashier_argv(int argc, char **argv, int *cashierNumber, int *serviceTime, int *breakTime, int *shmid)
{
	int i;
	int j;
	int s = 0;
	int b = 0;
	int m = 0;
	int c = 0;

	if ((argc != 9)) return 0;

	for (i = 2; i < 9; i+=2)
	{
		for (j = 0; j < strlen(argv[i]); j++)
		{
			if (!isdigit(argv[i][j])) return 0;
		} 
	}
	
	for (i = 1; i < 8; i+=2)
	{
		if (!strcmp(argv[i], "-s"))
		{
			s = 1;
			*serviceTime = atoi(argv[i+1]);
		}
		else if (!strcmp(argv[i], "-b"))
		{
			b = 1;
			*breakTime = atoi(argv[i+1]);
		}
		else if (!strcmp(argv[i], "-m"))
		{
			m = 1;
			*shmid = atoi(argv[i+1]);
		}
		else if (!strcmp(argv[i], "-i"))
		{
			c = 1;
			*cashierNumber = atoi(argv[i+1]);
		}

	}

	if ( (m+b+s+c) != 4)	return 0;

	return 1;
}


int main(int argc, char **argv)
{
	int cashierNumber;
	int serviceTime;
	int breakTime;
	int shmid;
	int err;
	int order;
	int client;
	struct SharedMemory *shm;

	if (!load_cashier_argv(argc, argv, &cashierNumber, &serviceTime, &breakTime, &shmid))
	{
		printf("ERROR: cashier initiation should follow the below format:\n");
		printf("\t./cashier -i cashierNumber -s serviceTime -b breakTime -m shmid\n");
		
		return 0;
	}

	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);

	printf("Cashier %d starts work.\n", cashierNumber);

	srand(getpid());
	while((*shm).open == 1)
	{
		if ((*shm).orders[cashierNumber] == 0)
		{	
			int temp = rand()%breakTime + 1;
			printf("Cashier %d has no client, taking a %d second break...\n", cashierNumber, temp);
			sleep(temp);
		}
		else
		{	
			int temp = rand()%serviceTime + 1;
			order = (*shm).orders[cashierNumber];
			client = (*shm).ordering_clients[cashierNumber];
			printf("Client %d orders %d. Cashier %d takes the order in %d seconds...\n", client, order, cashierNumber, temp);
			//record and send server
			sleep(temp);
			sem_wait(&(*shm).sp4);
			(*shm).orders[cashierNumber] = 0;
			(*shm).ordering_clients[cashierNumber] = 0;
			sem_post(&(*shm).sp4);
			printf("Cashier %d finishes with client %d...\n", cashierNumber, client);
			sem_post(&(*shm).sp3);
		}
	}

	printf("Cashier %d finishes work.\n", cashierNumber);
	err = shmdt((void *)shm); if (err == -1) perror ("Detachment.");

	return 0;
}