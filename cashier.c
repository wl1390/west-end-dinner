#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "macro.h"

int load_cashier_argv(int argc, char **argv, int *cashierNumber, int *serviceTime, int *breakTime)
{
	int i;
	int j;
	int s = 0;
	int b = 0;
	int c = 0;

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
		else if (!strcmp(argv[i], "-i"))
		{
			c = 1;
			*cashierNumber = atoi(argv[i+1]);
		}

	}

	if ( (b+s+c) != 3)	return 0;

	return 1;
}


int main(int argc, char **argv)
{
	int cashierNumber;
	int serviceTime;
	int breakTime;
	int err;
	int order;
	int client;
	struct SharedMemory *shm;
	int shmid;


	if (!load_cashier_argv(argc, argv, &cashierNumber, &serviceTime, &breakTime))
	{
		printf("ERROR: cashier initiation should follow the below format:\n");
		printf("\t./cashier -i cashierNumber -s serviceTime -b breakTime\n");
		
		return 0;
	}

	getSharedMemory(&shmid);
	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);

	// printf("Cashier %d starts work.\n", cashierNumber);

	srand(getpid());
	while((*shm).open == 1)
	{	
		sleep(1);
		if ((*shm).ordering[cashierNumber] == 0)
		{	
			int temp = rand()%breakTime + 1;
			// printf("Cashier %d has no client, taking a %d second break...\n", cashierNumber, temp);
			sleep(temp);
		}
		else
		{	
			int temp = rand()%serviceTime + 1;
			order = (*shm).ordering[cashierNumber];
			client = (*shm).ordering_clients[cashierNumber];
		
			printf("Cashier %d takes client %d in %d seconds...\n", cashierNumber, client, temp);
			sleep(temp);
			sem_wait(&(*shm).sp4);
			(*shm).ordering[cashierNumber] = 0;
			(*shm).ordering_clients[cashierNumber] = 0;

		
			printf("Cashier %d finishes with client %d...\n", cashierNumber, client);
			
			sem_post(&(*shm).sp3);

			sem_wait(&(*shm).sp5);
			FILE *fp;
			fp = fopen("database","a");
			fprintf(fp, "%d,%d,%f,\n",client, order,(*shm).menu.price[order]);
			fclose(fp);
			sem_post(&(*shm).sp5);
		}
	}

	// printf("Cashier %d finishes work.\n", cashierNumber);
	err = shmdt((void *)shm); if (err == -1) perror ("Detachment.");

	return 0;
}