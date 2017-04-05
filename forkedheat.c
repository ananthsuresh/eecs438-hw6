//Ananth Suresh//
//axs1264//
//HW 6 Resubmission//
//MultiProcess version//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h> // For fork
#include <fcntl.h> // For shm_open flags
#include <sys/mman.h> // For shm_open and mmap
#include <sys/wait.h> // For wait

int COLUMNS;
int ROWS;

//   helper routine
void track_progress(int iteration, double* Temperature, int COLUMNS, int ROWS);
double* setup_memory(char *name, int size);

int main(int argc, char *argv[]) {
    COLUMNS = atoi(argv[1]);
    ROWS = atoi(argv[2]);
    int i, j;                                            // grid indexes
    int max_iterations = 300;                              // number of iterations
    int iteration;                                   // current iteration
    int testing = 1;

    const int SIZE = (COLUMNS + 2) * (ROWS + 2) * sizeof(double); // size for the array
    char *nameA = "Temperature";
    char *nameB = "Temperature_last";
    double *Temperature = setup_memory(nameA, SIZE);//temperature grid
    double *Temperature_last = setup_memory(nameB, SIZE);//temperature_past grid

    //initialize
    for(i = 0; i <= ROWS+1; i++){
     for (j = 0; j <= COLUMNS+1; j++){
            Temperature_last[i * (COLUMNS + 2) + j] = 0.0;
        }
    }

    // these boundary conditions never change throughout run
    // set left side to 0 and right to a linear increase
    for(i = 0; i <= ROWS+1; i++) {
        Temperature_last[i * (COLUMNS + 2) ] = 0.0;
        Temperature_last[i * (COLUMNS + 2) + COLUMNS + 1] = (100.0/ROWS)*i;
       
    }
    // set top to 0 and bottom to linear increase
    for(j = 0; j <= COLUMNS+1; j++) {
        Temperature_last[j] = 0.0;
        Temperature_last[(ROWS+1) * (COLUMNS + 2) + j] = (100.0/COLUMNS)*j;  
    }       
   
    
    //Prints initial Grid
    if(testing == 1){
        printf("Initial Grid");
        for (i = 0; i < COLUMNS + 2; i++) {
            for (j = 0; j < ROWS + 2; j++){
                printf("%.1f ", Temperature_last[i*(COLUMNS + 2)+j]);
            }
            printf("\n");
        }
    }

    pid_t pid;
    //pid_t pid1;
    struct timeval start_time, stop_time, elapsed_time;  // timers
    gettimeofday(&start_time,NULL); // Unix timer
    // do until error is minimal or until max steps
    for (iteration = 1; iteration <= max_iterations; iteration++) {

        int nStart,nStop;
        pid = fork();

        if (pid > 0) { // Parent
        nStart = 1;
        nStop = (ROWS / 2) - 1;
        }
        
        else { // Child
            nStart = ROWS / 2;
            nStop = ROWS;
        }
        
        // main calculation: average my four neighbors
        for(i = nStart; i <= nStop; i++) {
            for(j = 1; j <= COLUMNS; j++) {
                Temperature[i * (COLUMNS + 2) + j] = 0.25 * (Temperature_last[(i+1) * (COLUMNS + 2) + j] +
                                            Temperature_last[(i-1) * (COLUMNS + 2) + j] +
                                            Temperature_last[i * (COLUMNS + 2) + j+1] +
                                            Temperature_last[i * (COLUMNS + 2) + j-1]);
            }
        }
        
        wait(NULL);
        if(pid == 0){
            return(0);
        }
       
        //int nStart,nStop;
        pid = fork();
        if (pid > 0) { // Parent
            nStart = 1;
            nStop = (ROWS / 2) - 1;
        }
        
        else { // Child
            nStart = ROWS / 2;
            nStop = ROWS;
        }
        // copy grid to old grid for next iteration and find latest dt
        
        for(i = nStart; i <= nStop; i++){
            for(j = 1; j <= COLUMNS; j++){
              Temperature_last[i * (COLUMNS + 2) + j] = Temperature[i * (COLUMNS + 2) + j];
            }
        }
        wait(NULL);//end of the second two-process calculaiton
    
        if(pid == 0){
            return(0);
        }

        // periodically print test values
        if(testing == 1){
            int iteration_jump = 10; // Use to print fewer snapshots
            if((iteration % iteration_jump) == 0) {
                track_progress(iteration, Temperature, COLUMNS, ROWS);
            }
        }
    }
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time); // Unix time subtract routine
    fflush(stdout);
    printf("\n%f seconds\n", elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    //Prints final Grid
    if(testing == 1){
        printf("Final Grid");
        for (i = 0; i < COLUMNS + 2; i++) {
            for (j = 0; j < ROWS + 2; j++){
                printf("%.1f ", Temperature_last[i*(COLUMNS + 2)+j]);
            }
            printf("\n");
        }
    }
    // Release memory
    if (shm_unlink(nameA) == -1 | shm_unlink(nameB) == -1) {
        printf("Error removing memory\n");
        exit(-1);
    }

}


// print diagonal in bottom right corner where most action is
void track_progress(int iteration, double* Temperature, int COLUMNS, int ROWS) {
    int i,j;
    printf("---------- Iteration number: %d ------------\n", iteration);
    for(i = ROWS-3; i <= ROWS; i++) {
        printf("[%d,%d]: %5.2f  ", i, i, Temperature[i * (COLUMNS + 2) + i]);
    }
    printf("\n");

}

// Helper function to setup memory
double* setup_memory(char *name, int size) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, size);
    double *m = (double*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (m == MAP_FAILED) {
        printf("Map failed\n");
        exit(-1);
    }
    return m;
}

