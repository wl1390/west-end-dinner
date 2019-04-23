#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "macro.h"

int load_client_argv(int argc, char **argv, int *itemId, int *eatTime)
{
	int i;
	int j;
	int t = 0;
	int e = 0;

	if ((argc != 5)) return 0;

	for (i = 2; i < 5; i+=2)
	{
		for (j = 0; j < strlen(argv[i]); j++)
		{
			if (!isdigit(argv[i][j])) return 0;
		} 
	}
	
	for (i = 1; i < 4; i+=2)
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

	}

	if ( (t+e) != 2)	return 0;

	return 1;
}

/* client calls this function when entering the restaurant */
int clientEnter(struct SharedMemory *shm, int pid)
{	
	/* Client must enter when the restaurent is open and not full */
	if ((*shm).restaurant_is_open !=1||((*shm).clients_in_restaurant >= MaxNumOfClients))
		return 0;

	sem_wait(&(*shm).sp1);

	if ((*shm).clients_in_restaurant == 0)
		sem_wait(&(*shm).sp2);

	(*shm).clients_in_restaurant++;

	(*shm).total_clients++;
	sem_post(&(*shm).sp1);

	return 1;
}

/* client calls this function when leaving the restaurant */
int clientLeave(struct SharedMemory *shm, int pid)
{	
	sem_wait(&(*shm).sp1);

	(*shm).clients_in_restaurant--;

	if ((*shm).clients_in_restaurant == 0)
		sem_post(&(*shm).sp2);

	sem_post(&(*shm).sp1);

	return 1;
}

int order(struct SharedMemory *shm, int pid, int itemId)
{
	int cashier;
	int i;

	srand(pid);
	cashier = rand() % MaxNumOfCashiers;

	sem_wait(&(*shm).sp3);
	sem_wait(&(*shm).sp4);

	/* Assign client to a free cashier */
	for (i = 0; i < MaxNumOfCashiers; i++)
	{	
		cashier = (cashier + 1) % MaxNumOfCashiers;

		if ((*shm).cashier_desk[cashier][0] == 0)
		{
			(*shm).cashier_desk[cashier][0] = itemId;
			(*shm).cashier_desk[cashier][1] = pid;
			break;
		}
	}

	sem_post(&(*shm).sp4);

	return cashier;
}

int main(int argc, char **argv)
{
	struct SharedMemory *shm;
	int shmid;

	int itemId;
	int eatTime;

	int cashier;
	
	time_t start_time;
	time_t end_time;
	int eating_time;

	FILE *fp;
	pid_t pid;

	pid = getpid();
	srand(pid);

	/* Client must be initialized with all parameters */
	if (!load_client_argv(argc, argv, &itemId, &eatTime))
	{
		printf("ERROR: client initiation should follow the below format:\n");
		printf("\t./client -i itemId -e eatTime\n");
		return 0;
	}

	/* Attaching shared memory */
	getSharedMemory(&shmid);
	shm = (struct SharedMemory *) shmat(shmid, (void*) 0, 0);

	/* Client must order item on the menu */
	if (itemId < 1 || itemId > (*shm).menu.item_count) 
	{
		printf("Item %d is not on the menu. Client leaving...\n", itemId);
		return 0;
	}

	/* Client enters the restaurant */
	if (clientEnter(shm, pid) == 0)
	{
		printf("Restaurant full. Client [%d] leaves.\n", pid);
		return 0;
	}

	printf("client %d enters.\n", pid);
	start_time = time(NULL);


	printf("Client %d is waiting for available cashier...\n", pid);
	cashier = order(shm, pid, itemId);

	printf("Client %d is talking to cashier %d...\n", pid,cashier);
	while((*shm).cashier_desk[cashier][0] != 0) continue;
	sem_post(&(*shm).sp4);

	printf("Client %d finishes ordering\n", pid);

	int temp = rand() % ((*shm).menu.maxTime[itemId] - (*shm).menu.minTime[itemId]) + (*shm).menu.minTime[itemId];

	printf("Client %d is waiting for food ...\n", pid);
	sleep(temp);

	printf("Client %d is waiting for server ...\n", pid);
	sem_wait(&(*shm).sp6);
	(*shm).server_busy = 1;
	printf("Server is serving food ...\n");
	while((*shm).server_busy != 0) continue;
	sem_post(&(*shm).sp6);

	printf("Client %d is eating ...\n", pid);
	eating_time = rand()%eatTime + 1;
	sleep(eating_time);

	printf("Client %d leaves...\n", pid);
	clientLeave(shm, pid);
	
	/* Send time data to "database_client" */
	end_time = time(NULL);
	sem_wait(&(*shm).sp7);
	fp = fopen("database_client","a");
	fprintf(fp, "%d,%d,%d,\n", pid, (int)(end_time - start_time), (int)(end_time - start_time) - eating_time);
	fclose(fp);
	sem_post(&(*shm).sp7);

	/* Detach segment */
	if (shmdt((void *)shm) == -1) perror ("Detachment.");
	return 0;
}
