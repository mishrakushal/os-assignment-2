#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef long long int lli;

#define MAX_THREADS 3

/* GLOBAL VARIABLES */
lli I, J, K;
lli **matrix1, **matrix2, **output;
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
int read_matrix (const char*, lli row, lli cols);
void *thread_read (void *);
void create_threads_and_read (int, int, int, file_read_data, FILE *);


/* single-threaded function to read matrix from the text file  */
int read_matrix (const char* filename, lli rows, lli cols) {

    /* get input file number from the file name  */
    char *filename_as_string;
    strcpy (filename_as_string, filename);
    int file_number = filename_as_string[2] - '0';

    // lli rows, cols;
    // if (file_number == 1) {
    //     rows = I;
    //     cols = J;
    //     matrix1 = (lli *) malloc (rows * cols * sizeof (lli));
    // } else {
    //     rows = J;
    //     cols = K;
    //     matrix2 = (lli *) malloc (rows * cols * sizeof (lli));
    // }

    FILE *fp;
    fp = fopen (filename, "r");
    if (fp == NULL) {
        return EXIT_FAILURE;
    }

    for (lli i = 0; i < rows; ++i) {
        for (lli j = 0; j < cols; ++j) {
            if (file_number == 1) {
                fscanf(fp, "%lld", &matrix1[i * rows + j]);
            } else {
                fscanf(fp, "%lld", &matrix2[i * rows + j]);
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
    // printf ("Matrix: %lld\n", *file.matrix);

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
            printf ("(i, j) = (%lld, %lld) ", i, j);
            // printf ("index = %lld\tvalue: ", (i * max_rows) + j);
            // printf ("index = %lld\tvalue: ", (i * max_rows) + j);
            // fscanf(fp, "%lld", &file.matrix[i][j]);
            // fscanf(fp, "%lld", (lli *)file.matrix[i][j]);
            // fscanf(fp, "%lld", &file.matrix[i][j]);
            lli temp;
            fscanf(fp, "%lld", &temp);
            printf ("%lld | ", temp);
            // file.matrix[i][j] = temp;
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

int main (int argc, char **argv) {
    
    FILE *fp = fopen (data.filename, "r");
    /* there must be exactly 7 cmd line args  */
    if (argc != 7) {
        printf ("Incorrect input...\n");
        return (EXIT_FAILURE);
    }

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
    read_matrix (in1,I,J);
    read_matrix (in2,J,K);

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
    for (int max_thread_count = 1; max_thread_count <= MAX_THREADS; ++max_thread_count) {
        file_read_data file = {.filename = in1, .cols = J, .matrix = matrix1, .max_rows = I};
        if(fp==NULL) break;
        create_threads_and_read (I, J, max_thread_count, file, fp);
    }

    // printf ("After reading matrix 1...\n");
    // for (lli row = 0; row < I; ++row) {
    //     for (lli col = 0; col < J; ++col) {
    //         printf ("%lld ", matrix1[row * I + col]);
    //     }
    //     printf ("\n");
    // }

    return 0;
}