#include	<semaphore.h>


#define MaxNumOfClients	  50
#define MaxNumOfCashiers  5
#define MaxNumOfServer	  1

struct SharedMemory
{
	int numOfClients;
	int clients[MaxNumOfClients];
	sem_t clientsp;
};


// All operations on the shared memory happens here 
// All functions needs lock

int initiateSharedMemory(struct SharedMemory *shm){

	int i;
	int retval;

	(*shm).numOfClients = 0;

	for (i = 0; i < MaxNumOfClients; i++){
	
		(*shm).clients[i] = 0;
	}

	retval = sem_init(&((*shm).clientsp),1,1);
	if (retval != 0) {
		perror("Couldn't initialize.");
		exit(1);
	}

	return 1;
}

int destroySharedMemory(struct SharedMemory *shm){

	//destrop shared memory

	return 1;
}

int addClient(int pid, struct SharedMemory *shm)
{	
	int i;
	int retval;

	//acqure lock
	retval = sem_wait(&((*shm).clientsp));
	printf("retval wait is %d\n", retval);

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

	//Release Lock
	retval = sem_post(&((*shm).clientsp));
	printf("retval post is %d\n", retval);

	

	return 1;
}

int removeClient(int pid, struct SharedMemory *shm)
{	
	int i;
	int retval;
	//acqure lock
	retval = sem_wait(&((*shm).clientsp));
	printf("retval wait is %d\n", retval);

	
	(*shm).numOfClients--;

	for(i = 0; i < MaxNumOfClients; i++)
	{
		if ((*shm).clients[i] == pid)
		{
			(*shm).clients[i] = 0;
			break;
		}
	}

	//Release Lock
	retval = sem_post(&((*shm).clientsp));
	printf("retval post is %d\n", retval);

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


	

