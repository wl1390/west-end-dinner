#include <stdio.h>
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>

#include "macro.h"

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
	FILE *fp;
	char buffer[64];

	/* Initializing shared variables to 0 */

	(*shm).restaurant_is_open = 0;
	(*shm).total_clients = 0;
	(*shm).clients_in_restaurant = 0;
	(*shm).server_busy = 0;

	for (i = 0; i < MaxNumOfCashiers; i++)
	{
		(*shm).cashier_desk[i][0] = 0;
		(*shm).cashier_desk[i][1] = 0;
	}

	/* Initializing semaphores */
	sem_init(&(*shm).sp1,1,1);
	sem_init(&(*shm).sp2,1,1);
	sem_init(&(*shm).sp3,1,MaxNumOfCashiers);
	sem_init(&(*shm).sp4,1,1);
	sem_init(&(*shm).sp5,1,1);
	sem_init(&(*shm).sp6,1,1);
	sem_init(&(*shm).sp7,1,1);

	/* Load menu from "menu.csv" */

	fp = fopen("menu.csv", "r");

	i = 0;

	bzero(buffer, 64);
	fscanf(fp,"%s", buffer);
	
	while(strcmp(buffer, ""))
	{
		i++;

		char *argv[6];
    	parser(buffer, argv);

		(*shm).menu.price[i] = atoi(argv[2]);
		(*shm).menu.minTime[i] = atoi(argv[3]);
		(*shm).menu.maxTime[i] = atoi(argv[4]);
		
 		free(argv[0]);

		bzero(buffer, 64);
		fscanf(fp,"%s", buffer);
	}

	(*shm).menu.item_count = i;

	fclose(fp);

	return 1;
}

int destroySemaphores(struct SharedMemory *shm)
{
	sem_destroy(&(*shm).sp1);
	sem_destroy(&(*shm).sp2);
	sem_destroy(&(*shm).sp3);
	sem_destroy(&(*shm).sp4);
	sem_destroy(&(*shm).sp5);
	sem_destroy(&(*shm).sp6);
	sem_destroy(&(*shm).sp7);
	return 1;
}

void analyze(struct SharedMemory *shm)
{
	int i, j;
	FILE *fp;
	char buffer[64];

	int data[(*shm).total_clients][5];
	int averageWaitingTime = 0;
	int totalMoney = 0;
	int item[(*shm).menu.item_count + 1];
	int mostPopularRevenue = 0;

	int mostPopular[5];

	for (i = 0; i < 5; i++)
	{
		mostPopular[i] = 0;
	}
//no need for a double list change!!!

	bzero(item, ((*shm).menu.item_count + 1) * sizeof(int));
	for (i = 0; i  < (*shm).total_clients; i++)
		bzero(data[i], 5*sizeof(int));
	
	/* Reading data from "database_cashier" */
	fp = fopen("database_cashier", "r");	
	
	bzero(buffer, 64);
	fscanf(fp,"%s", buffer);

	while(strcmp(buffer, ""))
	{
		char *argv[3];
    	parser(buffer, argv);

    	i = -1;
    	for (j = 0; j <  (*shm).total_clients; j++)
    	{
    		if (i == -1 && data[j][0] == 0) 
    			i = j;

    		if (data[j][0] == atoi(argv[0]))
    		{
    			i = j;
    			break;
    		}
    	}

		data[i][0] = atoi(argv[0]);
    	data[i][1] = atoi(argv[1]);
    	data[i][2] = atoi(argv[2]);
	
 		free(argv[0]);
		bzero(buffer, 64);
		fscanf(fp,"%s", buffer);
	}

	fclose(fp);

	/* Reading data from "database_client" */
	fp = fopen("database_client", "r");

	bzero(buffer, 64);
	fscanf(fp,"%s", buffer);

	while(strcmp(buffer, ""))
	{
		char *argv[3];
    	parser(buffer, argv);
	
    	i = -1;
    	for (j = 0; j <  (*shm).total_clients; j++)
    	{
    		if (i == -1 && data[j][0] == 0) 
    			i = j;
    		
    		if (data[j][0] == atoi(argv[0]))
    		{
    			i = j;
    			break;
    		}
    	}

		data[i][0] = atoi(argv[0]);
    	data[i][3] = atoi(argv[1]);
    	data[i][4] = atoi(argv[2]);
	
 		free(argv[0]);
		bzero(buffer, 64);
		fscanf(fp,"%s", buffer);
	}

	fclose(fp);

	remove("database_client");
	remove("database_cashier");

	/* Create aggregated data file and running analysis */
	/* Data in format client id, order, price, eatingTime, totalTime */
	fp = fopen("data", "w");
	
	for (i = 0; i < (*shm).total_clients; i++) 
	{
		fprintf(fp,"%d %d %d %d %d\n", data[i][0],data[i][1],data[i][2],data[i][3],data[i][4]);
		averageWaitingTime += data[i][4];
		totalMoney += data[i][2];
		item[data[i][1]]++;
	}
	
	fclose(fp);

	averageWaitingTime = averageWaitingTime/(*shm).total_clients;

	int min = 0;
	int minItemPosition = 0;

	for (i = 1; i <= (*shm).menu.item_count; i++)
	{
		if (item[i] > min)
		{
			mostPopular[minItemPosition] = i;
			min = item[mostPopular[0]];
			minItemPosition = 0;

			for (j = 1; j < 5; j++)
			{
				if (item[mostPopular[j]] < min)
				{
					min = item[mostPopular[j]];
					minItemPosition = j;
				}
			}
		}

	}

	/* printing analytics */
	printf("###################################\n");
	printf("Average waiting time is %d second.\n", averageWaitingTime);
	printf("Total Revenue is %.2f.\n", (double)totalMoney/100);
	printf("total clients is %d.\n", (*shm).total_clients);
	printf("Most popular items are ");
	for (i = 0; i < 5; i++)
	{
		if (mostPopular[i] != 0)
		{
			printf("%d ", mostPopular[i]);
			mostPopularRevenue += item[mostPopular[i]] * (*shm).menu.price[mostPopular[i]];		
		}
	}
	printf("\n");
	printf("They generated %.2f in total.\n", (double)mostPopularRevenue/100);
	printf("###################################\n");

	return;

}

int main(int argc, char **argv)
{	
	struct SharedMemory *shm;
	int id = 0;
	int i, j;
	FILE *fp;

	/* Create shared memory */
	id = shmget(IPC_PRIVATE, 1, 0666|IPC_CREAT); if (id == -1) perror ("Creation");
	shm = (struct SharedMemory *) shmat(id, (void*)0, 0);
	initiateSharedMemory(shm);
	
	/* Create relevant files */
	fp = fopen("memoryId", "w");
	fprintf(fp, "%d",id);
	fclose(fp);
	fp = fopen("database_client", "w");
	fclose(fp);
	fp = fopen("database_cashier", "w");
	fclose(fp);

	printf("Opening the restaurant\n");
	sem_wait(&(*shm).sp2);
	(*shm).restaurant_is_open = 1;
	sem_post(&(*shm).sp2);

	printf("Creating %d cashiers and server\n", MaxNumOfCashiers);
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
			char cahsierNumber[16];
			char serviceTime[16];
			char breakTime[16];
			printf("starting cahsier %d...\n", i);
			sprintf(cahsierNumber, "%d", i);
			sprintf(serviceTime, "%d", SERVICETIME);
			sprintf(breakTime, "%d", BREAKTIME);
			char *cashierargs[] = {"./cashier", "-i", cahsierNumber,"-s", serviceTime, "-b", breakTime, NULL};
			execvp(cashierargs[0], cashierargs);
			exit(1);
		}
	}

	getchar();
	sem_wait(&(*shm).sp2);
	(*shm).restaurant_is_open = 0;
	sem_post(&(*shm).sp2);

	while(wait(NULL) > 0);

	printf("Closing the restaurant. Analyzing data ...\n");
	analyze(shm);
	
	/* Remove segment */
	destroySemaphores(shm);
	if (shmctl(id, IPC_RMID, 0) == -1) perror ("Removal.");

	return 0;
}