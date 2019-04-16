#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>

int load_cashier_argv(int argc, char **argv, int *serviceTime, int *breakTime, int *shmid)
{
	int i;
	int j;
	int s = 0;
	int b = 0;
	int m = 0;

	if ((argc != 7)) return 0;

	for (i = 2; i < 7; i+=2)
	{
		for (j = 0; j < strlen(argv[i]); j++)
		{
			if (!isdigit(argv[i][j])) return 0;
		} 
	}
	
	for (i = 1; i < 6; i+=2)
	{
		if (!strcmp(argv[i], "-s"))
		{
			s = 1;
			*serviceTime = atoi(argv[i+1]);
		}
		else if (!strcmp(argv[i], "-b"))
		{
			b = 1;
			*breakTime = atoi(argv[i+1]);
		}
		else if (!strcmp(argv[i], "-m"))
		{
			m = 1;
			*shmid = atoi(argv[i+1]);
		}

	}

	if ( (m+b+s) != 3)	return 0;

	return 1;
}


int main(int argc, char **argv)
{
	int serviceTime;
	int breakTime;
	int shmid;

	if (!load_cashier_argv(argc, argv, &serviceTime, &breakTime, &shmid))
	{
		printf("ERROR: cashier initiation should follow the below format:\n");
		printf("\t./cashier -s serviceTime -b breakTime -m shmid\n");
		
		return 0;
	}

	printf("%d\n%d\n%d\n", serviceTime, breakTime, shmid);

	//while restaurant open
		//if no people in the line, break
		//pick a client
		//take the order
		//give it to the server

	return 0;
}