#include <stdio.h>
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>

#include "macro.h"

int main(int argc, char **argv)
{	
	int id = 0;
	int err = 0;
	int i;
	struct SharedMemory *shm;
	FILE *fp;

	id = shmget(IPC_PRIVATE, 1, 0666|IPC_CREAT); if (id == -1) perror ("Creation");
	
	shm = (struct SharedMemory *) shmat(id, (void*)0, 0);

	initiateSharedMemory(shm);
	
	fp = fopen("memoryId", "w");
	fprintf(fp, "%d",id);
	fclose(fp);
	fp = fopen("database_client", "w");
	fclose(fp);
	fp = fopen("database_cashier", "w");
	fclose(fp);


	printf("Allocated Shared Memory with ID: %d\n",(int)id);

	printf("Opening the restaurant\n");

	sem_wait(&(*shm).sp2);
	(*shm).open = 1;
	sem_post(&(*shm).sp2);

	printf("Creating cashiers and server\n");

	if (fork() == 0)
	{	

		printf("starting server...\n");
		char *serverargs[] = {"./server", NULL};
		execvp(serverargs[0], serverargs);
		exit(1);
	}

	for (i = 0; i < MaxNumOfCashiers; i++)
	{
		if (fork() == 0)
		{
			printf("starting cahsier %d...\n", i);
			char cahsierNumber[16];
			char serviceTime[16];
			char breakTime[16];
			sprintf(cahsierNumber, "%d", i);
			sprintf(serviceTime, "%d", SERVICETIME);
			sprintf(breakTime, "%d", BREAKTIME);
			char *cashierargs[] = {"./cashier", "-i", cahsierNumber,"-s", serviceTime, "-b", breakTime, NULL};
			execvp(cashierargs[0], cashierargs);
			exit(1);
		}
	}

	getchar();

	printf("Closing the restaurant\n");
	sem_wait(&(*shm).sp2);
	(*shm).open = 0;
	sem_post(&(*shm).sp2);

	while(wait(NULL) > 0);


	printf("all worker done ...\n");
	

	//ANALYZE DATA

	printf("start ANALYZE\n");

	FILE *data_client, *data_cashier;
	char buffer[64];
	data_client = fopen("database_client", "r");
	data_cashier = fopen("database_cashier", "r");	
	
	int k;
	int data[(*shm).totalClients][5];
	for (k = 0; k  < (*shm).totalClients; k++)
		bzero(data[k], 5*sizeof(int));

	bzero(buffer, 64);
	fscanf(data_cashier,"%s", buffer);

	while(strcmp(buffer, ""))
	{
		printf("buffer read is now %s\n", buffer);
		char *argv[3];
    	parser(buffer, argv);

    	int j;
    	k = -1;
    	for (j = 0; j <  (*shm).totalClients; j++)
    	{
    		if (k == -1 && data[j][0] == 0) 
    			k = j;

    		if (data[j][0] == atoi(argv[0]))
    		{
    			k = j;
    			break;
    		}
    	}

		data[k][0] = atoi(argv[0]);
    	data[k][1] = atoi(argv[1]);
    	data[k][2] = atoi(argv[2]);
	
 		free(argv[0]);
		bzero(buffer, 64);
		fscanf(data_cashier,"%s", buffer);
	}

	fclose(data_cashier);

	bzero(buffer, 64);
	fscanf(data_client,"%s", buffer);

	while(strcmp(buffer, ""))
	{
		printf("buffer read is now %s\n", buffer);
		char *argv[3];
    	parser(buffer, argv);
		
		int j;
    	k = -1;
    	for (j = 0; j <  (*shm).totalClients; j++)
    	{
    		if (k == -1 && data[j][0] == 0) 
    			k = j;
    		
    		if (data[j][0] == atoi(argv[0]))
    		{
    			k = j;
    			break;
    		}
    	}

		data[k][0] = atoi(argv[0]);
    	data[k][3] = atoi(argv[1]);
    	data[k][4] = atoi(argv[2]);
	
 		free(argv[0]);
		bzero(buffer, 64);
		fscanf(data_client,"%s", buffer);
	}

	fclose(data_client);
	
	int averageWaitingTime = 0;
	int totalMoney = 0;
	int item[(*shm).menu.count];
	bzero(item, (*shm).menu.count * sizeof(int));


	FILE *hey;
	hey = fopen("data", "w");
	


	for (k = 0; k < (*shm).totalClients; k++) 
	{
		fprintf(hey,"%d %d %d %d %d\n", data[k][0],data[k][1],data[k][2],data[k][3],data[k][4]);
		averageWaitingTime += data[k][4];
		totalMoney += data[k][2];
		item[data[k][1]]++;

		
	}
	
	fclose(hey);

	averageWaitingTime = averageWaitingTime/(*shm).totalClients;

	int topFive[5];
	int topFiveCount[5];
	bzero(topFive, 5*sizeof(int));	
	bzero(topFiveCount, 5*sizeof(int));

	for (k = 0; k < (*shm).menu.count; k++)
	{
		int x;
		for (x = 0; x < 5; x++)
		{
			if (topFiveCount[x]<item[k])
			{
				topFiveCount[x] = item[k];
				topFive[x] = k;
				break;
			}
		}
	}

	int y;
	for (y=0;y<5;y++){
		printf("top 5 is %d\n",topFive[y]);
		topFiveCount[y] = topFiveCount[y] * (*shm).menu.price[topFive[y]];
		printf("moeny made is is %d\n", topFiveCount[y]);
	}

	printf("averageWaitingTime is %d\n", averageWaitingTime);
	printf("total money is %d\n", totalMoney);
	printf("total clients is %d\n", (*shm).totalClients);

	/* Remove segment */
	destroySharedMemory(shm);
	err = shmctl(id, IPC_RMID, 0); if (err == -1) perror ("Removal.");

	return 0;
}
