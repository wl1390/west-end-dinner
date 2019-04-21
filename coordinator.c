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
	struct SharedMemory *shm;

	id = shmget(IPC_PRIVATE, 1, 0666|IPC_CREAT); if (id == -1) perror ("Creation");
	
	shm = (struct SharedMemory *) shmat(id, (void*)0, 0);

	initiateSharedMemory(shm);
	printf("Allocated Shared Memory with ID: %d\n",(int)id);

	//load the menu into shared memory
	//load server and cashiers processes
	
	operateRestaurant(shm, 1);
	// while(1) //some condition
	// {
		getchar();
	// 	inspect(shm);
	// }

	operateRestaurant(shm, 0);

	//wait for all cashier and server?
	//collect data 



	/* Remove segment */
	destroySharedMemory(shm);
	err = shmctl(id, IPC_RMID, 0); if (err == -1) perror ("Removal.");

	return 0;
}
