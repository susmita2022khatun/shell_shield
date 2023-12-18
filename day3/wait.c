#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>

int main(){
    //create a child
    pid_t child = fork();//fork returns 0
    int exitStatus , childPid;

    if(child == 0){
        //display childnrunning
        //put the child to sleep
        printf("child: running and pid:%d\n", getpid());

        sleep(5);

        //exit the child
        exit(40);
    }else if(child > 0){
        //parent 
        printf("parent: running with pid:%d\n", getpid());
        childPid = wait(&exitStatus);
        printf("parent: child finished it had the pid: %d, exitstatus:%d\n", getpid(), WEXITSTATUS(exitStatus));

    }else{
        printf("cant fork\n");
        exit(1);
    }
    return 0;
}