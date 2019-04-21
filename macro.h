#include <semaphore.h>
#include <fcntl.h>

#define MaxNumOfClients	  	50
#define MaxNumOfCashiers  	5
#define MaxNumOfServer	  	1
#define OpenTime			

struct SharedMemory
{
	int open;
	int numOfClients;
	int clients[MaxNumOfClients];
	sem_t sp;
	sem_t sp2;
};


// All operations on the shared memory happens here 

int initiateSharedMemory(struct SharedMemory *shm){

	int i;
	
	(*shm).numOfClients = 0;
	(*shm).open = 0;

	for (i = 0; i < MaxNumOfClients; i++){
	
		(*shm).clients[i] = 0;
	}

	printf("initializing semaphores\n");
	sem_init(&(*shm).sp,1,1);
	sem_init(&(*shm).sp2,1,1);

	return 1;
}

int destroySharedMemory(struct SharedMemory *shm){

	sem_destroy(&(*shm).sp);

	return 1;
}

int addClient(struct SharedMemory *shm, int pid)
{	
	int i;

	if ((*shm).open !=1||((*shm).numOfClients >= MaxNumOfClients))
		return 0;

	sem_wait(&(*shm).sp);
	printf("acquired lock\n");

	if ((*shm).numOfClients == 0)
		sem_wait(&(*shm).sp2);

	printf("adding client %d to list...\n", pid);

	(*shm).numOfClients++;

	for(i = 0; i < MaxNumOfClients; i++)
	{
		if ((*shm).clients[i] == 0)
		{
			(*shm).clients[i] = pid;
			break;
		}	
	}
	
	getchar();
	sem_post(&(*shm).sp);
	printf("releasd lock\n");	

	return 1;
}

int removeClient(struct SharedMemory *shm, int pid)
{	
	int i;

	sem_wait(&(*shm).sp);
	printf("acquired lock\n");

	printf("removing client %d to list...\n", pid);

	(*shm).numOfClients--;

	for(i = 0; i < MaxNumOfClients; i++)
	{
		if ((*shm).clients[i] == pid)
		{
			(*shm).clients[i] = 0;
			break;
		}
	}

	getchar();

	if ((*shm).numOfClients == 0)
		sem_post(&(*shm).sp2);

	sem_post(&(*shm).sp);
	printf("releasd lock\n");

	return 1;
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


	

