#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef long long int lli;

#define THREAD_NO 3

lli I, J, K;

lli matrix2[3][2]={{1,2},{1,2},{1,2}};
lli matrix1[4][3]={{1,2,3},{5,6,7},{8,9,10},{11,12,13}};

// lli **matrix1, **matrix2;
lli **output;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    lli* row;
    int row_no;
    int start_column;
    int end_column;
} working_cells_data;

void *thread_multiply (void *arg) {
    printf ("In thread_multiply function\n");
    working_cells_data data = *(working_cells_data*) arg;

    // ? --- DEBUG STATEMENTS ---
    // printf ("start_row: %d\n", start_row);
    // printf ("end_row: %d\n", end_row);
    // printf ("max_row: %lld\n", max_rows);
    // printf ("cols: %d\n", cols);
    for(int n=data.start_column; n<=data.end_column; n++)
    {
        output[data.row_no][n]=0;
        for(int i=0; i<J; i++)
        {
            output[data.row_no][n] += data.row[i]*matrix2[i][n];
        }
        printf ("calculated cell: %lld\n", output[data.row_no][n]);
    }
    

    printf ("Thread execution completed successfully\n");
}

void create_threads_and_multiply (int max_thread_count, lli* row, int row_no) {

    pthread_t threads[max_thread_count];    // {t1, t2}
    int cells_done = 0;
    // printf ("max_thread_count: %d\n", max_thread_count);
    for (int i = 0; i < max_thread_count; ++i) {
        int cells_per_thread = (K / max_thread_count);

        // last thread may have some extra lines to read
        if(i == max_thread_count - 1) {
            cells_per_thread += (K % max_thread_count);
        }

        printf ("cells done: %d\n", cells_done);
        working_cells_data work_data = {
            .row = row,
            .row_no = row_no,
            .start_column = cells_done,
            .end_column = (cells_done+cells_per_thread-1)
        };

        pthread_mutex_lock(&lock);
        if (pthread_create(&threads[i], NULL, thread_multiply, &work_data)) {
            fprintf(stderr, "pthread_create failed!\n");
            exit (EXIT_FAILURE);
        }
        pthread_mutex_unlock(&lock);
        pthread_join (threads[i], NULL);
        cells_done += cells_per_thread;
        }
}

int main()
{
    //get i,j,k
    I=4;
    J=3;
    K=2;

    //initialize output matrix
    output = (lli **) malloc (I * sizeof (lli *));
    for (lli _i = 0; _i < I; ++_i) {
		output[_i] = (lli *) malloc(K * sizeof(lli));
    }

    //get 2d matrix
    

    //get rows and then keep calling and sending it to function
    //write output matrix

    


    for(int i=0;i<I;i++)
    {
        if(THREAD_NO>J)
            create_threads_and_multiply (J, matrix1[i], i);
        else
            create_threads_and_multiply (THREAD_NO, matrix1[i], i);
        
    }



    return 0;
}