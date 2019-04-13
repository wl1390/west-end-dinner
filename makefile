default: coordinator

coordinator.o: coordinator.c 
	gcc -g -c coordinator.c -o coordinator.o

coordinator: coordinator.o
	gcc -g coordinator.o -o coordinator

clean:
	rm -f coordinator.o
	rm -f coordinator