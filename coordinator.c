#include <stdio.h>
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>

#include "macro.h"

int main(int argc, char **argv)
{	
	int id = 0;
	int err = 0;
	struct SharedMemory *shm;

	id = shmget(IPC_PRIVATE, 1, 0666|IPC_CREAT); if (id == -1) perror ("Creation");
	
	shm = (struct SharedMemory *) shmat(id, (void*)0, 0);

	initiateSharedMemory(shm);
	printf("Allocated Shared Memory with ID: %d\n",(int)id);
 	getchar();

 	inspect(shm);
 	getchar();

 	destroySharedMemory(shm);
	/* Remove segment */
	//TODO is this destrop this segment?
	err = shmctl(id, IPC_RMID, 0); if (err == -1) perror ("Removal.");
	


	//open the restaurant 
	//create shared memory
	//load the menu into shared memory
	//load server and cashiers processes
	//start acccepting clients and start counter
	//while counter on
		//monitor client lists

	//stop accepting new clients and wait for all client process to finish
	//close the restaurant
	//collect data 
	//wait for all cashier and server
	//release sys resource
	//exit

	return 0;
}