#include <semaphore.h>
#include <fcntl.h>

#define MaxNumOfClients	  	50
#define MaxNumOfCashiers  	3
#define SERVICETIME			7
#define BREAKTIME			10	
#define SERVERTIME			2

struct Menu
{	int item_count;
	int price[64];
	int minTime[64];
	int maxTime[64];
};

struct SharedMemory
{
	struct Menu menu;

	int restaurant_is_open;
	int total_clients;
	int clients_in_restaurant;
	int cashier_desk[MaxNumOfCashiers][2];	
	int server_busy;

	sem_t sp1;
	sem_t sp2;
	sem_t sp3;
	sem_t sp4;
	sem_t sp5;
	sem_t sp6;
	sem_t sp7;
};

/* Get Shared Memory Id */
void getSharedMemory(int *shmid)
{
	char buffer[16];
	FILE *fp;
	fp = fopen("memoryId", "r");
	fscanf(fp,"%s", buffer);
	*shmid = atoi(buffer);
	return;
}