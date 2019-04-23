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
	struct SharedMemory *shm;
	int shmid;

	int cashierNumber;
	int serviceTime;
	int breakTime;

	int order;
	int client;
	
	int temp;
	FILE *fp;

	if (!load_cashier_argv(argc, argv, &cashierNumber, &serviceTime, &breakTime))
	{
		printf("ERROR: cashier initiation should follow the below format:\n");
		printf("\t./cashier -i cashierNumber -s serviceTime -b breakTime\n");
		
		return 0;
	}

	/* Attaching shared memory */
	getSharedMemory(&shmid);
	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);

	srand(getpid());

	while((*shm).restaurant_is_open == 1)
	{	
		sleep(1);

		/* If no client is waiting at the cashier desk, cashier takes a break; */
		if ((*shm).cashier_desk[cashierNumber][0] == 0)
		{	
			temp = rand()%breakTime + 1;
			sleep(temp);
		}

		/* Cashier takes a clients order */
		else
		{	
			temp = rand()%serviceTime + 1;
			order = (*shm).cashier_desk[cashierNumber][0];
			client = (*shm).cashier_desk[cashierNumber][1];
		
			printf("Cashier %d takes client %d. \n", cashierNumber, client);
			sleep(temp);
			sem_wait(&(*shm).sp4);
			(*shm).cashier_desk[cashierNumber][0] = 0;
			(*shm).cashier_desk[cashierNumber][1] = 0;
		
			printf("Cashier %d finishes client %d: %d second.\n", cashierNumber, client, temp);
			sem_post(&(*shm).sp3);

			/* Cashier sends record to "database_cashier" */
			sem_wait(&(*shm).sp5);
			fp = fopen("database_cashier","a");
			fprintf(fp, "%d,%d,%d,\n",client, order,(*shm).menu.price[order]);
			fclose(fp);
			sem_post(&(*shm).sp5);
		}
	}

	/* Detaching shared memory */
	if (shmdt((void *)shm) == -1) perror ("Detachment.");

	return 0;
}