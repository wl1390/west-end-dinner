#include <semaphore.h>
#include <fcntl.h>

#define MaxNumOfClients	  	50
#define MaxNumOfCashiers  	3
#define SERVICETIME			5
#define BREAKTIME			5		

struct SharedMemory
{
	int open;
	int numOfClients;
	int clients[MaxNumOfClients];
	int orders[MaxNumOfCashiers];
	int ordering_clients[MaxNumOfCashiers];
	sem_t sp1; //locks when client enters/leaves restaurant
	sem_t sp2; //locks when there is client in the restaurant
	sem_t sp3; //locks when there is no cashier available 
	sem_t sp4; //locks when client is ordering 

};


// All operations on the shared memory happens here 

int initiateSharedMemory(struct SharedMemory *shm){

	int i;
	
	(*shm).numOfClients = 0;
	(*shm).open = 0;

	for (i = 0; i < MaxNumOfClients; i++)
	{
		(*shm).clients[i] = 0;
	}

	for (i = 0; i < MaxNumOfCashiers; i++)
	{
		(*shm).orders[i] = 0;
		(*shm).ordering_clients[i] = 0;
	}

	printf("initializing semaphores\n");
	sem_init(&(*shm).sp1,1,1);
	sem_init(&(*shm).sp2,1,1);
	sem_init(&(*shm).sp3,1,MaxNumOfCashiers);
	sem_init(&(*shm).sp4,1,1);

	return 1;
}

int destroySharedMemory(struct SharedMemory *shm){

	sem_destroy(&(*shm).sp1);
	sem_destroy(&(*shm).sp2);
	sem_destroy(&(*shm).sp3);
	sem_destroy(&(*shm).sp4);

	return 1;
}

int clientEnter(struct SharedMemory *shm, int pid)
{	
	int i;

	if ((*shm).open !=1||((*shm).numOfClients >= MaxNumOfClients))
		return 0;

	sem_wait(&(*shm).sp1);

	if ((*shm).numOfClients == 0)
		sem_wait(&(*shm).sp2);

	(*shm).numOfClients++;

	for(i = 0; i < MaxNumOfClients; i++)
	{
		if ((*shm).clients[i] == 0)
		{
			(*shm).clients[i] = pid;
			break;
		}	
	}

	sem_post(&(*shm).sp1);

	return 1;
}

int clientLeave(struct SharedMemory *shm, int pid)
{	
	int i;

	sem_wait(&(*shm).sp1);

	(*shm).numOfClients--;

	for(i = 0; i < MaxNumOfClients; i++)
	{
		if ((*shm).clients[i] == pid)
		{
			(*shm).clients[i] = 0;
			break;
		}
	}

	if ((*shm).numOfClients == 0)
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

	printf("Client %d waiting for available cashier...\n", pid);
	sem_wait(&(*shm).sp3);
	sem_wait(&(*shm).sp4);

	for (i = 0; i < MaxNumOfCashiers; i++)
	{	
		cashier = (cashier + 1) % MaxNumOfCashiers;

		if ((*shm).orders[cashier] == 0)
		{
			(*shm).orders[cashier] = itemId;
			(*shm).ordering_clients[cashier] = pid;
			break;
		}
	}

	sem_post(&(*shm).sp4);

	printf("Client %d orders %d to cashier %d...\n", pid, itemId, cashier);

	return cashier;
}

void operateRestaurant(struct SharedMemory *shm, int mode)
{
	sem_wait(&(*shm).sp2);
	(*shm).open = mode;
	sem_post(&(*shm).sp2);
	return;
}

void inspect(struct SharedMemory *shm)
{	
	int i;
	printf("we have %d of clients.\n", shm->numOfClients);
	
	for(i = 0; i < MaxNumOfClients; i++)
	{
		if ((*shm).clients[i] != 0)
		{	
			printf("client %d is eating.\n", (*shm).clients[i]);
		}
	}

	return;
}


	

