#define MaxNumOfClients	  50
#define MaxNumOfCashiers  5
#define MaxNumOfServer	  1

struct ClientInfo
{
	int AcceptClients;
	int *ClientLine[MaxNumOfClients];
};