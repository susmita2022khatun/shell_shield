#include<stdio.h>//input and output
#include<stdlib.h>
#include<unistd.h>//getpid()
#include<sys/types.h>

int main(int argc, char *argv){
    //fork to create a new process
    pid_t pid = fork();

    //check if fork was successful
    if (pid == 0 ){
        //code for the child 
        printf("this is the child process and the pid is:%d\n", getpid());//shows the simultaneous run ofchild and parets
        exit(0);// terminate the child process
            }

            else if (pid >0){
                printf("this is the parent process and pid is :%d\n",getpid());
                //parent 
            }

            //error handling 
            else{
                printf("error while forking\n");
                exit(EXIT_FAILURE);
            }
return 0;


}