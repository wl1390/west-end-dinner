#include <stdio.h>
#include <ctype.h> 
#include <string.h>


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

	//get pid and output (shmid)

	//starting code goes here
	//cashier space
	//server space
	//dining table
	//cashier waiting queue
	//server waiting queue
	//entire client queue
	//internal statistics


	return 0;
}