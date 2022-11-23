#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
using namespace std;


void create_threads_and_multiply (int rows, int cols, int max_thread_count, file_read_data data) {

    FILE *fp;
    fp = fopen (data.filename, "r");
    if (fp == NULL) {
        printf ("Unable to read file %s\n", data.filename);
        exit (EXIT_FAILURE);
    }

    // ? --- DEBUG STATEMENT ---
    printf ("File %s opened successfully\n", data.filename);

    pthread_t threads[max_thread_count];    // {t1, t2}
    int lines_read = 0;
    printf ("max_thread_count: %d\n", max_thread_count);
    for (int i = 0; i < max_thread_count; ++i) {
        int lines_per_thread = (rows / max_thread_count);

        // last thread may have some extra lines to read
        if(i == max_thread_count - 1) {
            lines_per_thread += (rows % max_thread_count);
        }

        printf ("lines read: %d\n", lines_read);
        file_read_data file_read_thread_data = {
            .filename = data.filename, 
            .cols = cols, 
            .lines_per_thread = lines_per_thread, 
            .start_row = lines_read, 
            .max_rows = data.max_rows
        };

        if (pthread_create(&threads[i], NULL, thread_read, &file_read_thread_data)) {
            fprintf(stderr, "pthread_create failed!\n");
            exit (EXIT_FAILURE);
        }
        pthread_join (threads[i], NULL);
        lines_read += lines_per_thread;
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

int main()
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

    for (int max_thread_count = 1; max_thread_count <= MAX_THREADS; ++max_thread_count) {
        file_read_data file = {.filename = in1, .cols = J, .matrix = matrix1, .max_rows = I};
        create_threads_and_multiply (I, J, max_thread_count, file);
    }

    return 0;
}