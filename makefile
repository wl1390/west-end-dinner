all: coordinator cashier client server

coordinator.o: coordinator.c 
	gcc -g -c coordinator.c -o coordinator.o -lpthread

coordinator: coordinator.o
	gcc -g coordinator.o -o coordinator -lpthread

cashier.o: cashier.c 
	gcc -g -c cashier.c -o cashier.o

cashier: cashier.o
	gcc -g cashier.o -o cashier

client.o: client.c 
	gcc -g -c client.c -o client.o -lpthread

client: client.o
	gcc -g client.o -o client -lpthread

server.o: server.c 
	gcc -g -c server.c -o server.o

server: server.o
	gcc -g server.o -o server

clean:
	rm -f coordinator.o
	rm -f coordinator
	rm -f server.o
	rm -f server
	rm -f client.o
	rm -f client
	rm -f cashier.o
	rm -f cashier
