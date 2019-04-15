#include <stdio.h>
#include <ctype.h> 
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


int check_coordinator_argv(int argc, char **argv)
{
	int i;

	if ((argc != 3) || (strcmp(argv[1], "-n"))) return 0;
	
	for (i = 0; i < strlen(argv[2]); i++)
	{
		if (!isdigit(argv[2][i])) return 0;
	} 

	return 1;
}


int main(int argc, char **argv)
{
	if (!check_coordinator_argv(argc, argv))
	{
		printf("ERROR: initiation command should follow the below format:\n");
		printf("\t./coordinator -n MaxNumOfCashiers\n");
		
		return 0;
	}

	//create shared memory
	//load the menu into shared memory
	//wait for the day to complete
	//collect data 
	//release sys resource

	return 0;
}