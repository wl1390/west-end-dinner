#include <semaphore.h>
#include <fcntl.h>

#define MaxNumOfClients	  	50
#define MaxNumOfCashiers  	3
#define SERVICETIME			2
#define BREAKTIME			1	
#define SERVERTIME			1

struct Menu
{	int count;
	float price[64];
	int minTime[64];
	int maxTime[64];
};

struct SharedMemory
{
	int open;
	int numOfClients;
	int clients[MaxNumOfClients];
	int ordering[MaxNumOfCashiers];
	int ordering_clients[MaxNumOfCashiers];
	int start;
	int count;
	int orders[MaxNumOfClients];
	int waiting_clients[MaxNumOfClients];
	int ready;
	int waiting;
	int wantfood;
	struct Menu menu;
	sem_t sp1; //locks when client enters/leaves restaurant
	sem_t sp2; //locks when there is client in the restaurant
	sem_t sp3; //locks when there is no cashier available 
	sem_t sp4; //locks when client is ordering 
	sem_t sp5; 
	sem_t sp6; //locks to notify cleint waiting time
	sem_t sp7; // locks to give client food
	sem_t sp8;

};

void getSharedMemory(int *shmid)
{
	char buffer[16];
	FILE *fp;
	fp = fopen("memoryId", "r");
	fscanf(fp,"%s", buffer);
	*shmid = atoi(buffer);
	return;
}

// All operations on the shared memory happens here 

void parser(const char *cmdline, char **argv){
	char *ptr;
	char *sptr;
	int c = 0;
	int i;

	ptr = strdup(cmdline);

	while(*ptr && (*ptr == ' ')) ptr++;

	for (i = 0; i < strlen(ptr); i++){
		if (ptr[strlen(ptr) - i] != ','){
			ptr[strlen(ptr)] = '\0';
			break;
		}
	}

	sptr = strchr(ptr, ',');

	while(sptr){
		argv[c++] = ptr;
		*sptr = '\0';
		ptr = sptr + 1;
		while(*ptr && (*ptr == ' ')) ptr++;
		sptr = strchr(ptr, ',');
	}

	argv[c] = NULL;
	return;
}


int initiateSharedMemory(struct SharedMemory *shm){

	int i;
	
	(*shm).start = 0;
	(*shm).count = 0;
	(*shm).numOfClients = 0;
	(*shm).open = 0;
	(*shm).wantfood = 0;

	for (i = 0; i < MaxNumOfClients; i++)
	{
		(*shm).clients[i] = 0;
		(*shm).waiting_clients[i] = 0;
		(*shm).orders[i] = 0;
	}

	for (i = 0; i < MaxNumOfCashiers; i++)
	{
		(*shm).ordering[i] = 0;
		(*shm).ordering_clients[i] = 0;
	}

	printf("initializing semaphores\n");
	sem_init(&(*shm).sp1,1,1);
	sem_init(&(*shm).sp2,1,1);
	sem_init(&(*shm).sp3,1,MaxNumOfCashiers);
	sem_init(&(*shm).sp4,1,1);
	sem_init(&(*shm).sp5,1,1);
	sem_init(&(*shm).sp6,1,1);
	sem_init(&(*shm).sp7,1,1);
	sem_init(&(*shm).sp8,1,1);

	int p = 0;
	FILE *fp;
	char buffer[64];
	fp = fopen("menu.csv", "r");

	bzero(buffer, 64);
	fscanf(fp,"%s", buffer);

	while(strcmp(buffer, ""))
	{
		p++;

		char *argv[6];
    	parser(buffer, argv);
		(*shm).menu.price[p] = atof(argv[2]);
		(*shm).menu.minTime[p] = atoi(argv[3]);
		(*shm).menu.maxTime[p] = atoi(argv[4]);
		
 		free(argv[0]);
		bzero(buffer, 64);
		fscanf(fp,"%s", buffer);
	}

	(*shm).menu.count = p;

	fclose(fp);


	return 1;
}

int destroySharedMemory(struct SharedMemory *shm){

	sem_destroy(&(*shm).sp1);
	sem_destroy(&(*shm).sp2);
	sem_destroy(&(*shm).sp3);
	sem_destroy(&(*shm).sp4);
	sem_destroy(&(*shm).sp5);
	sem_destroy(&(*shm).sp6);
	sem_destroy(&(*shm).sp7);
	sem_destroy(&(*shm).sp8);
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

		if ((*shm).ordering[cashier] == 0)
		{
			(*shm).ordering[cashier] = itemId;
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


	

