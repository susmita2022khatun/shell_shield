#include<stdio.h>
#include<unistd.h>//process id
#include<stdlib.h>

int main (int argc, char *argv[]){
printf("hello from exec2\n");
printf("PID of the exec2 is :%d\n", getpid());//process id 
return 0;


}
