all: coordinator cashier

coordinator.o: coordinator.c 
	gcc -g -c coordinator.c -o coordinator.o

coordinator: coordinator.o
	gcc -g coordinator.o -o coordinator


cashier.o: cashier.c 
	gcc -g -c cashier.c -o cashier.o

cashier: cashier.o
	gcc -g cashier.o -o cashier


clean:
	rm -f coordinator.o
	rm -f coordinator
	rm -f cashier.o
	rm -f cashier