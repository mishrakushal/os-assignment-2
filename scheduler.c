#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef long long int lli;
lli I, J, K;

int main(int argc, char **argv)
{

    I = atoi(argv[1]);
    J = atoi(argv[2]);
    K = atoi(argv[3]);
    char in1[10], in2[10], out[10];
    char arg1[20], arg2[20], arg3[20];

    /* strcpy() is used to copy strings from cmd into char array */
    strcpy(in1, argv[4]);
    strcpy(in2, argv[5]);
    strcpy(out, argv[6]);
    snprintf(arg1, sizeof(arg1), "%lli", I);
    snprintf(arg2, sizeof(arg2), "%lli", J);
    snprintf(arg3, sizeof(arg3), "%lli", K);

    key_t key_sched1 = ftok("shmfile1", 5);
    int sched_shmid1 = shmget(key_sched1, 2 * sizeof(lli *), 0666 | IPC_CREAT);
    lli *sched_str1 = (lli *)shmat(sched_shmid1, 0, 0);

    key_t key_sched2 = ftok("shmfile2", 66);
    int sched_shmid2 = shmget(key_sched2, 2 * sizeof(lli *), 0666 | IPC_CREAT);
    lli *sched_str2 = (lli *)shmat(sched_shmid2, 0, 0);

    // printf("%lld %lld\n",sched_str1[0], sched_str1[1]);
    // printf("%lld %lld\n",sched_str2[0], sched_str2[1] + 1);

    double total_time_taken;

    FILE *fpt;
    fpt = fopen("quant2.csv", "w");
    if (fpt == NULL)
    {
        printf("Could not create CSV file\n");
        return EXIT_FAILURE;
    }
    printf("%lld %d \n", I, getpid());
    // printf("%lld", (char*)I);
    sched_str1[0]=1;
    sched_str2[0]=1;
    if (fork() == 0)
    {
        // printf("a\n");
        // printf("%lld %lld \n", I, getpid());
        // execl("./test.out", "./test.out", NULL);
        
        execlp("./p1Sched.out", "p1Sched.out", arg1, arg2, arg3, in1, in2, out, NULL);
        // printf("a done\n");
    }
    if (fork() == 0)
    {
        // printf("b\n");
        // printf("%lld %lld \n", I, getpid());
        
        execlp("./p2Sched.out", "p2Sched.out", arg1, arg2, arg3, in1, in2, out, NULL);
        // printf("b done\n");
    }
    printf("%d done\n",getpid());
    // return 0;
    // QUANT TIME 2ms
    struct timespec start_time = {0, 0}, end_time = {0, 0};
    //pause both processes
    kill(sched_str1[1],SIGSTOP); // we are NOT killing the processes.
    kill(sched_str2[1],SIGSTOP);

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    // kill(sched_str1[1],SIGCONT); 
    while (1)
    {
        printf("\n-------Entered while with pid %d-------\n", getpid());
        if (sched_str1[0] && sched_str2[0])
        {
            printf("if 1\n");
            kill(sched_str2[1], SIGSTOP); //STOP P2
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            total_time_taken = ((double)end_time.tv_sec + 1.0e-9*end_time.tv_nsec) - ((double)start_time.tv_sec + 1.0e-9*start_time.tv_nsec);
            printf("P2, %.9f \n", total_time_taken);
            fprintf(fpt,"P2, %.9f \n", total_time_taken);


            clock_gettime(CLOCK_MONOTONIC, &start_time);
            kill(sched_str1[1], SIGCONT); //LET P1 WORK
            usleep(1000);
            kill(sched_str1[1], SIGSTOP); //STOP P1
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            total_time_taken = ((double)end_time.tv_sec + 1.0e-9*end_time.tv_nsec) - ((double)start_time.tv_sec + 1.0e-9*start_time.tv_nsec);
            printf("P1, %.9f \n", total_time_taken);
            fprintf(fpt,"P1, %.9f \n", total_time_taken);


            clock_gettime(CLOCK_MONOTONIC, &start_time);
            kill(sched_str2[1], SIGCONT); //LET P2 WORK
            
            usleep(1000);
        }
        if (sched_str1[0] && !sched_str2[0])
        {
            printf("if 2\n");
            kill(sched_str1[1], SIGCONT);
        }

        if (sched_str2[0] && !sched_str1[0])
        {
            printf("if 3\n");
            kill(sched_str2[1], SIGCONT);
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            total_time_taken = ((double)end_time.tv_sec + 1.0e-9*end_time.tv_nsec) - ((double)start_time.tv_sec + 1.0e-9*start_time.tv_nsec);
        }
        if(!sched_str1[0] && !sched_str2[0])
        {
            printf("if 4\n");
            break;
        }
        
    }

    FILE *fpt1;
    fpt1 = fopen("quant1.csv", "w");
    if (fpt1 == NULL)
    {
        printf("Could not create CSV file\n");
        return EXIT_FAILURE;
    }

    // // QUANT TIME 1ms
    // while (sched_str1[0] && sched_str2[0])
    // {
    //     kill(sched_str2[1], SIGSTOP); // STOP P2
    //     clock_gettime(CLOCK_MONOTONIC, &start_time);
    //     kill(sched_str1[1], SIGCONT); // LET P1 WORK
    //     clock_gettime(CLOCK_MONOTONIC, &end_time);
    //     total_time_taken = ((double)end_time.tv_sec + 1.0e-9 * end_time.tv_nsec) - ((double)start_time.tv_sec + 1.0e-9 * start_time.tv_nsec);
    //     printf("P1, %.9f \n", total_time_taken);
    //     fprintf(fpt, "P1, %.9f \n", total_time_taken);
    //     usleep(1000);

    //     kill(sched_str1[1], SIGSTOP); // STOP P1
    //     clock_gettime(CLOCK_MONOTONIC, &start_time);
    //     kill(sched_str2[1], SIGCONT); // LET P2 WORK
    //     clock_gettime(CLOCK_MONOTONIC, &end_time);
    //     total_time_taken = ((double)end_time.tv_sec + 1.0e-9 * end_time.tv_nsec) - ((double)start_time.tv_sec + 1.0e-9 * start_time.tv_nsec);
    //     printf("P2, %.9f \n", total_time_taken);
    //     fprintf(fpt, "P2, %.9f \n", total_time_taken);
    //     usleep(1000);
    // }

    // if (sched_str2[0])
    // {
    //     kill(sched_str2[1], SIGCONT);
    // }
    // wait(NULL);
    wait(NULL);

    return 0;
}