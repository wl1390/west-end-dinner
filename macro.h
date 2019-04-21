#include <semaphore.h>
#include <fcntl.h>


#define MaxNumOfClients	  50
#define MaxNumOfCashiers  5
#define MaxNumOfServer	  1

struct SharedMemory
{
	int numOfClients;
	int clients[MaxNumOfClients];
	sem_t sp;
};


// All operations on the shared memory happens here 
// All functions needs lock

int initiateSharedMemory(struct SharedMemory *shm){

	int i;
	
	(*shm).numOfClients = 0;

	for (i = 0; i < MaxNumOfClients; i++){
	
		(*shm).clients[i] = 0;
	}

	printf("initializing semaphore value to 1\n");
	sem_init(&(*shm).sp,1,1);

	return 1;
}

int destroySharedMemory(struct SharedMemory *shm){

	sem_destroy(&(*shm).sp);

	return 1;
}

int addClient(int pid, struct SharedMemory *shm)
{	
	int i;
	sem_wait(&(*shm).sp);
	printf("acquired lock\n");
	printf("adding client %d to list...\n", pid);
	
	if ((*shm).numOfClients >= MaxNumOfClients)
	{
		printf("Restaurant full. Client [%d] leaving...\n", pid);
	}
	else
	{	
		(*shm).numOfClients++;

		for(i = 0; i < MaxNumOfClients; i++)
		{
			if ((*shm).clients[i] == 0)
			{
				(*shm).clients[i] = pid;
				break;
			}
		}

	}
	
	getchar();
	sem_post(&(*shm).sp);
	printf("releasd lock\n");	

	return 1;
}

int removeClient(int pid, struct SharedMemory *shm)
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
	sem_post(&(*shm).sp);
	printf("releasd lock\n");

	return 1;
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


	

