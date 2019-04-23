all: restaurant cashier client server

restaurant.o: restaurant.c 
	gcc -g -c restaurant.c -o restaurant.o -lpthread

restaurant: restaurant.o
	gcc -g restaurant.o -o restaurant -lpthread

cashier.o: cashier.c 
	gcc -g -c cashier.c -o cashier.o -lpthread

cashier: cashier.o
	gcc -g cashier.o -o cashier -lpthread

client.o: client.c 
	gcc -g -c client.c -o client.o -lpthread

client: client.o
	gcc -g client.o -o client -lpthread

server.o: server.c 
	gcc -g -c server.c -o server.o -lpthread

server: server.o
	gcc -g server.o -o server -lpthread

clean:
	rm -f restaurant.o
	rm -f restaurant
	rm -f server.o
	rm -f server
	rm -f client.o
	rm -f client
	rm -f cashier.o
	rm -f cashier
	rm -f memoryId
	rm -f database_client
	rm -f database_cashier
	rm -f data
