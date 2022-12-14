#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

typedef long long int lli;

#define THREAD_COUNT 4
#define THREAD_T 3
#define THREAD_F 1
#define MAX_CMD_LENGTH 100

/* GLOBAL VARIABLES */
lli I, J, K;
lli **matrix1, **matrix2, **output;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
key_t key_p1;
int p1_shmid;
lli *p1_str ;
// lli *matrix1, *matrix2, *output;

void create_threads_and_multiply (int max_thread_count, lli* row, int row_no);

/* USER-DEFINED DATA STRUCTURES */
typedef struct {
    /*
        FILENAME            := name of file to read from
        MATRIX              := name of matrix which needs to be populated
        START ROW           := reference to the line which is to be read
        MAX ROW             := max rows in the matrix
        COLS                := number of columns to be read
        LINES_PER_THREAD    := max lines a thread can read
    */
    char *filename;
    lli **matrix;
    int start_row;
    lli max_rows;       // CAN REMOVE
    lli cols;           // CAN REMOVE
    int lines_per_thread;
} file_read_data;

typedef struct {
    lli* row;
    int row_no;
    int start_column;
    int end_column;
} working_cells_data;

/* single-threaded function to read matrix from the text file  */
int read_matrix (const char* filename) {

    /* get input file number from the file name  */
    char filename_as_string[10];
    strcpy (filename_as_string, filename);
    int file_number = filename_as_string[2] - '0';

    lli rows, cols;
    if (file_number == 1) {
        rows = I;
        cols = J;
    } else {
        rows = J;
        cols = K;
    }

    FILE *fp;
    fp = fopen (filename, "r");
    if (fp == NULL) {
        return EXIT_FAILURE;
    }

    for (lli _i = 0; _i < rows; ++_i) {
        for (lli _j = 0; _j < cols; ++_j) {
            if (file_number == 1) {
                lli temp;
                fscanf(fp, "%lld", &temp);
                // fscanf(fp, "%lld", matrix1[_i][_j]);
                matrix1[_i][_j] = temp;
            } else {
                lli temp;
                fscanf(fp, "%lld", &temp);
                // fscanf(fp, "%lld", matrix2[_i][_j]);
                matrix2[_i][_j] = temp;
            }
        }
    }

    fclose (fp); 
    return EXIT_SUCCESS;
}


void *thread_read (void *arg) {
    printf ("In thread_read function\n");
    file_read_data file = *(file_read_data*) arg;

    FILE* fp;
    fp = fopen(file.filename, "r");
    if (fp == NULL) {
        printf ("Unable to read file %s\n", file.filename);
        exit (EXIT_FAILURE);
    }
    printf ("Read %s successfully\n", file.filename);
    // printf ("Matrix: %lld\n", **file.matrix);

    int start_row = file.start_row;
    int end_row = start_row + file.lines_per_thread;
    lli max_rows = file.max_rows;
    int cols = file.cols;

    // ? --- DEBUG STATEMENTS ---
    printf ("start_row: %d\n", start_row);
    printf ("end_row: %d\n", end_row);
    printf ("max_row: %lld\n", max_rows);
    printf ("cols: %d\n", cols);

    fseek (fp, start_row, SEEK_CUR);
    lli i = 0;

    char wait_temp;
    // loop to skip to next line
    while (i < start_row && fscanf (fp, "%c", &wait_temp) != EOF) {
        if (wait_temp == '\n') {
            i++;
        }
    }

    for (i = start_row; i < end_row ; ++i) {
        for (lli j = 0; j < cols; ++j) {
            // printf ("(i, j) = (%lld, %lld) ", i, j);
            // printf ("index = %lld\tvalue: ", (i * max_rows) + j);
            // printf ("index = %lld\tvalue: ", (i * max_rows) + j);
            // fscanf(fp, "%lld", &file.matrix[i][j]);
            // fscanf(fp, "%lld", (lli *)file.matrix[i][j]);
            // fscanf(fp, "%lld", &file.matrix[i][j]);
            lli temp;
            fscanf(fp, "%lld", &temp);
            printf ("%lld | ", temp);
        }
        printf ("\n");
    }

    printf ("Thread execution completed successfully\n");
    fclose (fp);
}

/* function to create threads and read text file  */
void create_threads_and_read (int rows, int cols, int max_thread_count, file_read_data data, FILE *fp) {

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

        pthread_mutex_lock(&lock);
        if (pthread_create(&threads[i], NULL, thread_read, &file_read_thread_data)) {
            fprintf(stderr, "pthread_create failed!\n");
            exit (EXIT_FAILURE);
        }
        pthread_mutex_unlock(&lock);
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

    // fclose (fp); 
}


void *thread_multiply (void *arg) {
    // printf ("In thread_multiply function\n");
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
        // printf ("calculated cell: %lld\n", output[data.row_no][n]);
    }
    

    // printf ("Thread execution completed successfully\n");
}

void shared_memory (file_read_data file) {
    /* get input file number from the file name  */
    char *filename_as_string;
    filename_as_string = (char *) malloc(10);
    strcpy (filename_as_string, file.filename);
    int file_number = filename_as_string[2] - '0';

    // shmget returns an identifier in shmid
    lli rows = file.max_rows;
    lli cols = file.cols;

    // for (int i = 0; i < I; ++i) {
    //     if(MAX_THREADS > J) {
    //         create_threads_and_multiply (J, matrix1[i], i);
    //     } else {
    //         create_threads_and_multiply (MAX_THREADS, matrix1[i], i);
    //     }
    // }

    if (file_number == 1)
    {
        
        key_t key1 = ftok(".", 100*100*100*100+1);
        if (key1 == -1) {
            printf ("key1 is -1\n");
        }

        // int shmid1 = shmget(key1, (rows) * sizeof (lli*), 0666|IPC_CREAT);
        // int shmid1 = shmget(key1, sizeof (lli) * (cols * rows), 0666|IPC_CREAT);
        int shmid1 = shmget(key1, sizeof (lli *) * rows * cols, 0666|IPC_CREAT);
        // int shmid1 = shmget(key1, 1024, 0666|IPC_CREAT);

        
        if (shmid1 == -1) {
            printf ("shmid1 is -1\n");
        }

        // str1 = (lli*) shmat(shmid1, 0, 0);
        // int (*str1)[rows];
        // lli *str1;
        lli *str1 = (lli *) shmat(shmid1, NULL, 0);
        // int shmid1 = shmget(key1, (rows) * sizeof (lli*), 0666|IPC_CREAT);

        lli k = 0;
        for (lli i = 0; i < rows; ++i){
            for (lli j = 0; j < cols; ++j){
                matrix1[i][j] = str1[k];
                k++;
            }
            if(THREAD_COUNT > J) {
                create_threads_and_multiply (J, matrix1[i], i);
            } else {
                create_threads_and_multiply (THREAD_COUNT, matrix1[i], i);
            }
            // printf ("\n");
        }

        // shmdt(str1);
        // shmctl (shmid1, IPC_RMID, NULL);
    }
    else
    {
        key_t key2 = ftok(".", 17);
        if (key2 == -1) {
            printf ("key2 is -1\n");
        }

        // int shmid1 = shmget(key1, (rows) * sizeof (lli*), 0666|IPC_CREAT);
        // int shmid1 = shmget(key1, sizeof (lli) * (cols * rows), 0666|IPC_CREAT);
        int shmid2 = shmget(key2, sizeof (lli *) * rows * cols, 0666|IPC_CREAT);
        // int shmid1 = shmget(key1, 1024, 0666|IPC_CREAT);

        
        if (shmid2 == -1) {
            printf ("shmid2 is -1\n");
        }

        // str1 = (lli*) shmat(shmid1, 0, 0);
        // int (*str1)[rows];
        // lli *str1;
        lli *str2 = (lli *) shmat(shmid2, NULL, 0);

        if (str2 == (lli *)-1) {
            printf ("str2 is -1\n");
        }


        printf ("Reading matrix 2 from IPC:\n");
        for (lli i = 0; i < rows; ++i) {
            for (lli j = 0; j < cols; ++j) {
                matrix2[i][j] = str2[i * cols + j];
            }
        }
    }
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

        // printf ("cells done: %d\n", cells_done);
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

int main (int argc, char **argv) {
    
    /* there must be exactly 7 cmd line args  */
    if (argc != 7) {
        printf ("Incorrect input...\n");
        return (EXIT_FAILURE);
    }

    key_t key_sched2 = ftok("shmfile2", 10);
    int sched_shmid2 = shmget(key_sched2, 2 * sizeof (lli *), 0666|IPC_CREAT);

    // key_p1 = ftok("p1_process", 66);
    // p1_shmid = shmget(key_p1, 1 * sizeof (lli *), 0666|IPC_CREAT);
    // p1_str = (lli*) shmat(p1_shmid, 0, 0);
    // printf ("P2 p1_str: %lld\n", p1_str[0]);

    key_p1 = ftok(".", 0);
    p1_shmid = shmget(key_p1, 1 * sizeof (lli *), 0666|IPC_CREAT);
    p1_str  = (lli*) shmat(p1_shmid, 0, 0);

    lli *sched_str2 = (lli*) shmat(sched_shmid2, 0, 0);
    sched_str2[0] = 1;
    // shmdt (sched_str2);
    sched_str2[1] = getpid();
    printf("pid of 2 : %lld", sched_str2[1]);

    /* BOILERPLATE CODE TO READ ARGS FROM CMD  */
    I = atoi (argv[1]);
    J = atoi (argv[2]);
    K = atoi (argv[3]);

    matrix1 = (lli **) malloc (I * sizeof (lli *));
    for (lli _i = 0; _i < I; ++_i) {
		matrix1[_i] = (lli *) malloc(J * sizeof(lli));
    }

    matrix2 = (lli **) malloc (J * sizeof (lli *));
    for (lli _j = 0; _j < J; ++_j) {
		matrix2[_j] = (lli *) malloc(K * sizeof(lli));
    }

    output = (lli **) malloc (I * sizeof (lli *));
    for (lli _i = 0; _i < I; ++_i) {
		output[_i] = (lli *) malloc(K * sizeof(lli));
    }
    /*
        int **a;
        Allocate memory to matrix
        a = (int **) malloc(m * sizeof(int *));
        for(int i=0; i<m; i++) {
            a[i] = (int *) malloc(n * sizeof(int));
        }
    */

    char in1[10], in2[10], out[10];

    /* strcpy() is used to copy strings from cmd into char array */
    strcpy(in1, argv[4]);
    strcpy(in2, argv[5]);
    strcpy(out, argv[6]);

    FILE *fpt;
    file_read_data file1 = {.filename = in1, .cols = J, .matrix = matrix1, .max_rows = I};
    file_read_data file2 = {.filename = in2, .cols = K, .matrix = matrix2, .max_rows = J};

    while(p1_str[0]<0)
    {
        continue;
    }
    printf ("Reading matrix 2 from IPC\n");
    shared_memory (file2);
    while(p1_str[0]<1)
    {
        continue;
    }
    printf ("SHM matrix 2 done\n");
    printf ("Reading matrix 1 from IPC\n");
    shared_memory (file1);
    printf ("SHM matrix 1 done\n");

    printf ("Matrix 1 from IPC...\n");
    for (lli row = 0; row < I; ++row) {
        for (lli col = 0; col < J; ++col) {
            printf ("%lld ", matrix1[row][col]);
        }
        printf ("\n");
    }

    printf ("Matrix 2 from IPC...\n");
    for (lli row = 0; row < J; ++row) {
        for (lli col = 0; col < K; ++col) {
            printf ("%lld ", matrix2[row][col]);
        }
        printf ("\n");
    }

    // for (int i = 0; i < I; ++i) {
    //     if(MAX_THREADS > J) {
    //         create_threads_and_multiply (J, matrix1[i], i);
    //     } else {
    //         create_threads_and_multiply (MAX_THREADS, matrix1[i], i);
    //     }
        
    // }

    printf ("FINAL OUTPUT...\n");
    for (lli row = 0; row < I; ++row) {
        for (lli col = 0; col < K; ++col) {
            printf ("%lld ", output[row][col]);
        }
        printf ("\n");
    }

    FILE *fptOut;
    fptOut = fopen(out, "w");
    if(fptOut == NULL) {
        printf ("Could not create output.txt file\n");
        return EXIT_FAILURE;
    }

    for (lli row = 0; row < I; ++row) {
        for (lli col = 0; col < K; ++col) {
            // fscanf(fp, "%lld", &temp);
            fprintf (fptOut, "%lld ", output[row][col]);
            //! printf ("%lld ", output[row][col]);
        }
        if (row != I - 1) {
            fprintf (fptOut, "\n");
        }
    }
    fclose (fptOut);
    printf ("Program success\n");

    sched_str2[0] = 0;
    shmdt (sched_str2);

    return 0;
}