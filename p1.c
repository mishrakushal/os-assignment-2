#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef long long int lli;

/* GLOBAL VARIABLES */
lli i, j, k;
lli *matrix1, *matrix2, *output;

/* single-threaded function to read matrix from the text file  */
int read_matrix (const char* filename) {

    /* get input file number from the file name  */
    char filename_as_string[10];
    strcpy (filename_as_string, filename);
    int file_number = filename_as_string[2] - '0';

    lli rows, cols;
    if (file_number == 1) {
        rows = i;
        cols = j;
        matrix1 = (lli *) malloc (rows * cols * sizeof (lli));
    } else {
        rows = j;
        cols = k;
        matrix2 = (lli *) malloc (rows * cols * sizeof (lli));
    }

    FILE *fp;
    fp = fopen (filename, "r");
    if (fp == NULL) {
        return EXIT_FAILURE;
    }

    for (lli _i = 0; _i < rows; ++_i) {
        for (lli _j = 0; _j < cols; ++_j) {
            if (file_number == 1) {
                fscanf(fp, "%lld", &matrix1[(_i * rows) + _j]);
            } else {
                fscanf(fp, "%lld", &matrix2[(_i * rows) + _j]);
            }
        }
    }

    fclose (fp); 
    return EXIT_SUCCESS;
}

int main (int argc, char **argv) {

    /* there must be exactly 7 cmd line args  */
    if (argc != 7) {
        printf ("Incorrect input...\n");
        return (EXIT_FAILURE);
    }

    /* BOILERPLATE CODE TO READ ARGS FROM CMD  */
    i = atoi (argv[1]);
    j = atoi (argv[2]);
    k = atoi (argv[3]);
    output = (lli *) malloc (i * k * sizeof (lli));

    char in1[10], in2[10], out[10];

    /* strcpy() is used to copy strings from cmd into char array */
    strcpy(in1, argv[4]);
    strcpy(in2, argv[5]);
    strcpy(out, argv[6]);

    /* READING VALUES FROM TXT FILE INTO MATRICES  */
    read_matrix (in1);
    printf ("After reading matrix 1...\n");
    for (lli row = 0; row < i; ++row) {
        for (lli col = 0; col < j; ++col) {
            printf ("%lld ", matrix1[row * i + col]);
        }
        printf ("\n");
    }

    read_matrix (in2);
    printf ("After reading matrix 2...\n");
    for (lli row = 0; row < j; ++row) {
        for (lli col = 0; col < k; ++col) {
            printf ("%lld ", matrix2[row * i + col]);
        }
        printf ("\n");
    }

    return 0;
}