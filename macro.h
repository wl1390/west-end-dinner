#define MaxNumOfClients	  50
#define MaxNumOfCashiers  5
#define MaxNumOfServer	  1

struct SharedMemory
{
	int numOfClients;
	int clients[MaxNumOfClients];
};

// All operations on the shared memory happens here 
// All functions needs lock

int initiateSharedMemory(struct SharedMemory *shm){

	int i;

	(*shm).numOfClients = 0;

	for (i = 0; i < MaxNumOfClients; i++){
	
		(*shm).clients[i] = 0;
	}

	return 1;
}

int addClient(int pid, struct SharedMemory *shm)
{	
	int i;

	//acqure lock
	if ((*shm).numOfClients >= MaxNumOfClients)
	{
		printf("Restaurant full. Client [%d] leaving...\n", pid);
	}
	else
	{	
		(*shm).numOfClients++;

		for(i = 0; i < MaxNumOfClients; i++)
		{
			if ((*shm).clients[i] != 0)
			{
				(*shm).clients[i] = pid;
				break;
			}
		}

	}

	//Release Lock

	return 1;
}

void inspect(struct SharedMemory *shm)
{	
	// int i;
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


	

