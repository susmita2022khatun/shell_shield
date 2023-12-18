#include<iostream>
#include<sys/types.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>

static int alarm_fired = 0;

void ding(int sig){
    alarm_fired = 1;
}

int main(){
    pid_t pid;
    std::cout<<"alarm starting!\n";//not using namespacestd thats why std
    pid = fork();

    switch(pid){
        case -1:
        {
        //error handling
            perror("fork failed\n");
            exit(1);
            break;
        }
        case 0:
        {
            //child block
            sleep(5);
            pid_t parentpid = getpid();

            kill(parentpid, SIGUSR1);
            exit(0);
            break;
        }
        default:
        {
            //parent
            std::cout<<"waiting for alarm to go off\n";
            (void) signal(SIGUSR1, ding);
            pause();
            if(alarm_fired)std::cout<<"alarm fired\n";
            std::cout<<"done\n";
            
        }
    }
    exit(0);
}