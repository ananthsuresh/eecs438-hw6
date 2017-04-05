all:
	gcc serialheat.c -lrt -o serialheat
	gcc threadedheat.c -fopenmp -o threadedheat
	gcc forkedheat.c -lrt -o forkedheat
