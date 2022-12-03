#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef long long int lli;

#define MAX_THREADS 3
pthread_t threads[MAX_THREADS]; // {t1, t2}
int cells_done = 0;

typedef struct {
    lli row[2];
    lli matrix2[2][1];
    lli **output;
    int cell_no;
    int j;
    int k;
} working_cells_data;

void *thread_multiply (void *arg) {
    printf ("In thread_multiply function\n");
    working_cells_data data = *(working_cells_data*) arg;

    // ? --- DEBUG STATEMENTS ---
    // printf ("start_row: %d\n", start_row);
    // printf ("end_row: %d\n", end_row);
    // printf ("max_row: %lld\n", max_rows);
    // printf ("cols: %d\n", cols);
    for(int i=0; i<data.j; i++)
        data.output[data.cell_no/data.k][data.cell_no%data.k] += data.row[i]*data.matrix2[i][data.cell_no%data.k];

    printf ("Thread execution completed successfully\n");
}

void create_threads_and_multiply (int i, int j, int k, lli* row, lli** matrix2, lli **output) {

    
    printf ("MAX_THREADS: %d\n", MAX_THREADS);
    for (int i = 0; i < MAX_THREADS; ++i) {
        int cells_per_thread = (i*k / MAX_THREADS);

        // last thread may have some extra lines to read
        if(i == MAX_THREADS - 1) {
            cells_per_thread += (i*k % MAX_THREADS);
        }

        printf ("lines read: %d\n", cells_done);
        working_cells_data work_data = {
            .row = row,
            .matrix2 = matrix2,
            .output = output,
            .cell_no = cells_done++,
            .j = j,
            .k = k
        };

        if (pthread_create(&threads[i], NULL, thread_multiply, &work_data)) {
            fprintf(stderr, "pthread_create failed!\n");
            exit (EXIT_FAILURE);
        }
        // pthread_join (threads[i], NULL);
    }
    // ? --- DEBUG STATEMENT ---
    printf ("Threads created\n");

    // ! --- JOIN THE THREADS ---
    for (int i = 0; i < MAX_THREADS; ++i) {
        // printf ("Thread %d joined\t", i);
    }
    printf ("\n");
    // ? --- DEBUG STATEMENT ---
    printf ("All threads joined\n\n");

    // ! --- KILL/CLOSE ALL THREADS ---

}

char* ipc_read()
{
	// ftok to generate unique key
	key_t key = ftok("shmfile",65);

	// shmget returns an identifier in shmid
	int shmid = shmget(key,1024,0666|IPC_CREAT);

	// shmat to attach to shared memory
	char *str = (char*) shmat(shmid,(void*)0,0);

	printf("Data read from memory: %s\n",str);
	
	//detach from shared memory
	shmdt(str);
	
	// destroy the shared memory
	shmctl(shmid,IPC_RMID,NULL);

    return str;
}

int main()
{
    //get i,j,k
    int i=4,j=3,k=2;

    //declare output matrix
    lli** output;

    //get 2d matrix
    lli matrix2[3][2]={{1,1},{1,1},{1,1}};

    //get rows and then keep calling and sending it to function
    //write output matrix

    
    lli matrix1[4][3]={{1,2,3},{5,6,7},{8,9,10},{11,12,13}};


    create_threads_and_multiply (i, j, k, row, matrix2, output);

    return 0;
}