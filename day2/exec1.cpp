#include<stdio.h>
#include<unistd.h>//process id
#include<stdlib.h>

int main(int argc, char *argv[]) {
    printf("PID OF exec1.c = %d\n", getpid());//process id

    char *args[] ={ "hello", "from" , "parent", NULL};// arguments for the execv() call

    //replace the current process with the exec2

    execv("./exec2", args);


//this line will not be reached if execv() is successful
printf("back to exec1");
return 0;



}