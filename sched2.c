#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

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

    pid_t pro1_pid = -1, pro2_pid = -1, sch_pid = -1;
    sch_pid = getpid();
    pro1_pid = fork(); // returns 0 to child and child-pid to parent.
    if (pro1_pid == 0) // if child (pro1)
    {
        execlp("./p1Sched.out", "p1Sched.out", arg1, arg2, arg3, in1, in2, out, NULL);
    }

    // parent
    pro2_pid = fork();
    if (pro2_pid == 0) // if child (pro2)
    {
        execlp("./p2Sched.out", "p2Sched.out", arg1, arg2, arg3, in1, in2, out, NULL);
    }

    printf("%d %d \n", pro1_pid, pro2_pid);

    int quantum_number = 0;

    //pause both processes
    kill(pro1_pid,SIGSTOP); // we are NOT killing the processes.
    kill(pro2_pid,SIGSTOP);
    
    kill(pro1_pid,SIGCONT); 
    while (1)
    {
        printf("QUANTUM NUMBER: %d for pid %d\n",quantum_number, getpid());
        if (quantum_number%2==0) //even implies pro1 should execute
        {
            kill(pro2_pid,SIGSTOP);   
            kill(pro1_pid,SIGCONT);
        }
        else
        {
            kill(pro1_pid,SIGSTOP);   
            kill(pro2_pid,SIGCONT);
        }
        quantum_number++;
        sleep(5);
    }
    wait(NULL);
    return 0;
}