gcc p1Sched.c -o p1Sched.out -pthread
gcc p2Sched.c -o p2Sched.out -pthread
gcc scheduler.c -o scheduler.out -pthread

./scheduler.out 8 3 1 in1.txt in2.txt out.txt

./p1Sched.out 8 3 1 in1.txt in2.txt out.txt
./p2Sched.out 8 3 1 in1.txt in2.txt out.txt
