#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef long long int lli;

#define MAX_THREADS 3

using namespace std;

typedef struct {
    lli **matrix1;
    lli **matrix2;
    lli **output;
    int start_cell;    
    int end_cell;
} working_cells_data;

void *thread_multiply (void *arg) {
    printf ("In thread_multiply function\n");
    working_cells_data data = *(working_cells_data*) arg;

    // ? --- DEBUG STATEMENTS ---
    printf ("start_row: %d\n", start_row);
    printf ("end_row: %d\n", end_row);
    printf ("max_row: %lld\n", max_rows);
    printf ("cols: %d\n", cols);

    for(int n=data.start_cell; n<data.end_cell; n++)
    {
        for(int i=0; i<data.j; i++)
            data.output[n/k][n%k] += data.matrix1[n/k][i]*data.matrix2[i][n%k];
    }

    printf ("Thread execution completed successfully\n");
    fclose (fp);
}

void create_threads_and_multiply (int i, int j, int k, int max_thread_count, lli **matrix1, lli **matrix2, lli **output) {

    pthread_t threads[max_thread_count];    // {t1, t2}
    int cells_done = 0;
    printf ("max_thread_count: %d\n", max_thread_count);
    for (int i = 0; i < max_thread_count; ++i) {
        int cells_per_thread = (i*k / max_thread_count);

        // last thread may have some extra lines to read
        if(i == max_thread_count - 1) {
            cells_per_thread += (i*k % max_thread_count);
        }

        printf ("lines read: %d\n", cells_done);
        working_cells_data work_data = {
            .matrix1 = matrix1,
            .matrix2 = matrix2,
            .output = output,
            .start_cell = cells_done,
            .end_cell = (cells_done+cells_per_thread-1)
        };

        if (pthread_create(&threads[i], NULL, thread_multiply, &work_data)) {
            fprintf(stderr, "pthread_create failed!\n");
            exit (EXIT_FAILURE);
        }
        pthread_join (threads[i], NULL);
        cells_done += cells_per_thread;
    }
    // ? --- DEBUG STATEMENT ---
    printf ("Threads created\n");

    // ! --- JOIN THE THREADS ---
    for (int i = 0; i < max_thread_count; ++i) {
        // printf ("Thread %d joined\t", i);
    }
    printf ("\n");
    // ? --- DEBUG STATEMENT ---
    printf ("All threads joined\n\n");

    // ! --- KILL/CLOSE ALL THREADS ---

    fclose (fp); 
}

int ipc_read()
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

    return str
}

int main()
{
    char *matrices = ipc_read();

    lli** matrix1;
    lli** matrix2;
    lli** output;


    for (int max_thread_count = 1; max_thread_count <= MAX_THREADS; ++max_thread_count) {
        create_threads_and_multiply (i, j, k, max_thread_count, matrix1, matrix2, output);
    }

    return 0;
}