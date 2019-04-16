#include <stdio.h>
#include <ctype.h> 
#include <string.h>
#include <stdlib.h>

//MaxNumOfClients
//MaxNumOfCashiers

int load_coordinator_argv(int argc, char **argv, int *MaxNumOfCashiers)
{
	int i;

	if ((argc != 3) || (strcmp(argv[1], "-n"))) return 0;
	
	for (i = 0; i < strlen(argv[2]); i++)
	{
		if (!isdigit(argv[2][i])) return 0;
	} 

	*MaxNumOfCashiers = atoi(argv[2]);

	return 1;
}


int main(int argc, char **argv)
{
	int MaxNumOfCashiers;

	if (!load_coordinator_argv(argc, argv, &MaxNumOfCashiers))
	{
		printf("ERROR: coordinator initiation should follow the below format:\n");
		printf("\t./coordinator -n MaxNumOfCashiers\n");
		
		return 0;
	}

	printf("%d\n", MaxNumOfCashiers);

	//create shared memory
	//load the menu into shared memory
	//wait for the day to complete
	//collect data 
	//release sys resource

	return 0;
}