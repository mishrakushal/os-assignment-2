#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

typedef long long int lli;
lli I=8, J=3, K=1;
char arg1[20], arg2[20], arg3[20];
char in1[10], in2[10], out[10];

int main(int argc, char **argv)
{
    strcpy(in1, "in1.txt");
    strcpy(in2, "in2.txt");
    strcpy(out, "out.txt");
    snprintf(arg1, sizeof(arg1), "%lli", I);
    snprintf(arg2, sizeof(arg2), "%lli", J);
    snprintf(arg3, sizeof(arg3), "%lli", K);
    execlp("./p1Sched.out", "p1Sched.out", arg1, arg2, arg3, in1, in2, out, NULL);
    printf("Not working\n");
    return 0;
}