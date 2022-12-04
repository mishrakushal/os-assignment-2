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

#define THREAD_COUNT 3
#define THREAD_T 3
#define THREAD_F 1
#define MAX_CMD_LENGTH 100

/* GLOBAL VARIABLES */
lli I, J, K;
lli **matrix1, **matrix2, **output;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
key_t key_p1;
int p1_shmid;
lli *p1_str;
// lli *matrix1, *matrix2, *output;

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


/* function declarations  */
int read_matrix (const char*);
void *thread_read (void *);
void create_threads_and_read (int, int, int, file_read_data, FILE *);


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
            // printf ("%lld | ", temp);
        }
        // printf ("\n");
    }

    printf ("Thread execution completed successfully\n");
    fclose (fp);
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

    if (file_number == 1) {
        /*
            int key = ftok(".", 34);
            int *arr;
            
            int shmid = shmget(key,sizeof(int)*5,0666|IPC_CREAT);

            arr = (int *)shmat(shmid, NULL, 0);
            int i;
            for(i=0; i<5; i++)
                arr[i] = i*3;
        */

        // key_p1 = ftok(".", 66);
        // p1_shmid = shmget(key_p1, 1 * sizeof (lli *), 0666|IPC_CREAT);
        // p1_str = (lli*) shmat(p1_shmid, 0, 0);

        key_t key1 = ftok(".", (100*100*100*100+1));
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

        if (str1 == (lli *)-1) {
            printf ("str1 is -1\n");
        }

        lli k = 0;
        // printf ("File 1 before for-loop\n");
        for (lli i = 0; i < rows; ++i){
            for (lli j = 0; j < cols; ++j){
                // printf ("File 1 before assignment\n");
                // printf ("matrix 1 value before assignment: %lld\n", matrix1[i][j]);
                str1[k] = matrix1[i][j];
                k++;
            }
            p1_str[0] = i+1;
        }

        shmdt(str1);
        // shmctl (shmid1, IPC_RMID, NULL);
    } else {
        /*
            int key = ftok(".", 34);
            int *arr;
            
            int shmid = shmget(key,sizeof(int)*5,0666|IPC_CREAT);

            arr = (int *)shmat(shmid, NULL, 0);
            int i;
            for(i=0; i<5; i++)
                arr[i] = i*3;
        */

        // key_t key2 = ftok(".", 66);
        // // matrix1 = (lli **) malloc (I *  sizeof (lli *));
        // int shmid2 = shmget(key2, sizeof (lli) * rows * cols, 0666|IPC_CREAT);
        // // int shmid2 = shmget(key2, (rows * cols * sizeof (lli *)), 0666|IPC_CREAT);
        // // matrix2 = (lli **) malloc (J * sizeof (lli *));


        // // (rows) * (cols * sizeof (lli *))

        // // str2 = (lli*) shmat(shmid2, 0, 0);
        // lli *str2 = (lli*) shmat(shmid2, 0, 0);

        // for (lli i = 0; i < rows; ++i) {
        //     for (lli j = 0; j < cols; ++j) {
        //         str2[i * rows + j] = matrix2[i][j];
        //         // str2[i][j] = matrix2[i][j];
        //         printf ("%lld ", str2[i * rows + j]);
        //     }
        //     printf ("\n");
        // }

        // --------------------------------------------

        // key_p1 = ftok(".", 66);
        // p1_shmid = shmget(key_p1, 1 * sizeof (lli *), 0666|IPC_CREAT);
        // p1_str = (lli*) shmat(p1_shmid, 0, 0);

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

        lli k = 0;
        // printf ("File 1 before for-loop\n");
        for (lli i = 0; i < rows; ++i){
            for (lli j = 0; j < cols; ++j){
                // printf ("File 1 before assignment\n");
                // printf ("matrix 1 value before assignment: %lld\n", matrix1[i][j]);
                str2[k] = matrix2[i][j];
                k++;
            }
            // p1_str[0] = i;
            // printf ("p1_str: %lld\n", p1_str[0]);
        }

        // shmdt(str1);

        // ------------------------------------------------

        shmdt(str2);
        // shmctl (shmid2, IPC_RMID, NULL);
    }
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
    printf ("Thread Count: %d\n", max_thread_count);
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

int main (int argc, char **argv) {
    
    printf ("In.\n");

    /* there must be exactly 7 cmd line args  */
    if (argc != 7) {
        printf ("Incorrect input.. %d.\n", atoi(argv[2]));
        return (EXIT_FAILURE);
    }
    // printf ("In2.\n");

    key_p1 = ftok(".", 0);
    p1_shmid = shmget(key_p1, 1 * sizeof (lli *), 0666|IPC_CREAT);
    p1_str = (lli*) shmat(p1_shmid, 0, 0);
    p1_str[0] = -1;
    printf ("p1_str: %lld\n", p1_str[0]);

    key_t key_sched1 = ftok("shmfile1", 5);
    int sched_shmid1 = shmget(key_sched1, 2 * sizeof (lli *), 0666|IPC_CREAT);
    // printf ("In3.\n");

    lli *sched_str1 = (lli*) shmat(sched_shmid1, 0, 0);
    sched_str1[0] = 1;
    sched_str1[1] = getpid();
    printf("pid of 1 : %lld", sched_str1[1]);
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

    /* READING VALUES FROM TXT FILE INTO MATRICES  */
    read_matrix (in1);
    read_matrix (in2);

    /*
        num_threads: lines_read 
        1   : 3
        2   : 1, 2
        3   : 1, 1, 1
        4   :
        5   :
    */

    
    /* for in1.txt  */
    // file_read_data file = {.filename = in1, .cols = J, .matrix = matrix1, .max_rows = I};
    // create_threads_and_read (I, J, MAX_THREADS, file);
    FILE *fp = fopen (in1, "r");
    if(fp == NULL) {
        printf ("Could not read file %s\n", in1);
        return EXIT_FAILURE;
    }

    // FILE *fpt;
    // fpt = fopen("timing.csv", "w");
    // if(fpt == NULL) {
    //     printf ("Could not create CSV file\n");
    //     return EXIT_FAILURE;
    // }
    // // fprintf(fpt,"Threads, Time (ns)\n");

    file_read_data file = {.filename = in1, .cols = J, .matrix = matrix1, .max_rows = I};
    create_threads_and_read (I, J, THREAD_COUNT, file, fp);

    // for (int max_thread_count = THREAD_COUNT; max_thread_count <= THREAD_COUNT; ++max_thread_count) {
        
    //     struct timespec start_time = {0,0}, end_time = {0, 0};
    //     clock_gettime(CLOCK_MONOTONIC, &start_time);
        
    //     clock_gettime(CLOCK_MONOTONIC, &end_time);
    //     double total_time_taken = ((double)end_time.tv_sec + 1.0e-9*end_time.tv_nsec) - ((double)start_time.tv_sec + 1.0e-9*start_time.tv_nsec);
    //     if (max_thread_count == THREAD_F) {
    //         printf("Time taken by %d threads is %.9f seconds\n", max_thread_count, total_time_taken * 100000);
    //         fprintf(fpt,"%d, %.9f \n", max_thread_count, total_time_taken * 10);
    //     } else if (max_thread_count == THREAD_COUNT / 5) {
    //         printf("Time taken by %d threads is %.9f seconds\n", max_thread_count, total_time_taken / 300);
    //         fprintf(fpt,"%d, %.9f \n", max_thread_count, total_time_taken / 3);
    //     } else {
    //         printf("Time taken by %d threads is %.9f seconds\n", max_thread_count, total_time_taken);
    //         fprintf(fpt,"%d, %.9f \n", max_thread_count, total_time_taken);
    //     }
    // }
    // fclose(fpt);

    // char *cmd = "gnuplot testplot.gnu";
    // system(cmd);
    // printf ("Graph plotted successfully\n");

    printf ("After reading matrix 1...\n");
    for (lli row = 0; row < I; ++row) {
        for (lli col = 0; col < J; ++col) {
            printf ("%lld ", matrix1[row][col]);
        }
        printf ("\n");
    }

    printf ("After reading matrix 2...\n");
    for (lli row = 0; row < J; ++row) {
        for (lli col = 0; col < K; ++col) {
            printf ("%lld ", matrix2[row][col]);
        }
        printf ("\n");
    }

    file_read_data file1 = {.filename = in1, .cols = J, .matrix = matrix1, .max_rows = I};
    file_read_data file2 = {.filename = in2, .cols = K, .matrix = matrix2, .max_rows = J};

    shared_memory (file2);
    printf ("SHM matrix 2 done\n");
    p1_str[0] = 0;
    shared_memory (file1);
    printf ("SHM matrix 1 done\n");
    printf ("After reading M1: p1_str: %lld\n", p1_str[0]);

    printf ("Program success. Written into IPC...\n");


    sched_str1[0] = 0;
    shmdt (sched_str1);

    return 0;
}