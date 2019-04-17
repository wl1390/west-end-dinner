#define MaxNumOfClients	  50
#define MaxNumOfCashiers  5
#define MaxNumOfServer	  1

struct Client
{	
	pid_t pid;
	int itemId;
	int status;
};

struct SharedMemory
{
	int NumOfClients;
	struct Client *clients[MaxNumOfClients];
};

static struct Client empty = {0, 0 ,0};


// All operations on the shared memory happens here 
// All functions needs lock

int initiateSharedMemory(struct SharedMemory *shm){

	int i;

	(*shm).NumOfClients = 0;

	for (i = 0; i < MaxNumOfClients; i++){
	
		(*shm).clients[i] = &empty;
	}

	return 1;
}

int addClient(struct Client *client, struct SharedMemory *shm)
{	
	int i;

	//acqure lock
	if ((*shm).NumOfClients >= MaxNumOfClients)
	{
		printf("Restaurant full. Client [%d] leaving...\n", client->pid);
	}
	else
	{	
		(*shm).NumOfClients++;
		// (*shm).clients[0] = client;

		for(i = 0; i < MaxNumOfClients; i++)
		{
			if ((*shm).clients[i] != &empty)
			{
				(*shm).clients[i] = client;
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
	printf("we have %d of clients.\n", shm->NumOfClients);
	printf("client %d.\n", (*shm).clients[0]->pid);

	// for(i = 0; i < MaxNumOfClients; i++)
	// {
	// 	if ((*shm).clients[i] != NULL)
	// 	{	
	// 		printf("client %d, itemId %d, status %d.\n", (*shm).clients[i]->pid,(*shm).clients[i]->itemId, (*shm).clients[i]->status);
	// 	}
	// }

	return;
}


	

