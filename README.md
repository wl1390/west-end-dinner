# west-end-dinner
operating system project to simulate west end restaurant operation

### Running Simulation
* compile the program using `make`
* start the restaurant program using `./restaurant`
* start clients using `./client -i [itemId] -e [eatingTime]`
* to stop the restaurant, hit `ENTER` in the terminal and the process will finishes after all clients leave and all cashiers finish break.

### Code Structure 
**restaurant.c**: initializes shared memory. It spawns server and cashier processes and analyzes data in the end.

**server.c**: serves food to clients one by one. 

**cashier.c**: takes order from client and record it in database.

**client.c**: comes and goes.

### Semaphores
**sem_t sp1**: clients wait and release sp1 when they enter and leave restaurant.

**sem_t sp2**: first client to arrive acquires sp2 and last client to leave release sp2. Restaurant waits for sp2 to close.

**sem_t sp3**: clients wait for sp3 for available cashier.

**sem_t sp4**: clients wait for sp4 to order.

**sem_t sp5**: cashiers wait for sp5 to record data to file.

**sem_t sp6**: clients wait for sp6 for server to become available.

**sem_t sp7**: clients wait for sp7 to record data to file.
