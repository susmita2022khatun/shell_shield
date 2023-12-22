#include "parser.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#define  concat(a,b) (a"" b)
#define CGROUP_MEM "/sys/fs/cgroup/cntr/memory.max"
#define CGROUP_CPU "/sys/fs/cgroup/cntr/cpu.max"
#define CGROUP_SUBTREE "/sys/fs/cgroup/cgroup.subtree_control"
#define REQ_CGROUP "/sys/fs/cgroup/cntr"
using namespace std;


int isOK(int status,const char* msg){
    if(status==-1){
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return status;
}

void WRITE(const char* path,const char* value){
    int fd = open(path,O_WRONLY | O_APPEND);
    if(fd == -1){
        cout<<"Error in opening\n";
        exit(1);
    }
    ssize_t bytes = write(fd,value,strlen(value));
    if(bytes==-1){
        cout<<"error in writing\n";
        exit(1);
    }
    close(fd);

}
char* stack_mem(){
    const int stacksize = 65*1024;
    auto *stack = new (nothrow) char[stacksize];
    if(stack==nullptr){
        cout<<"Can't allocate memory";
        exit(EXIT_FAILURE);
    }
    return stack+stacksize;
}
void cloneProcess(int (*function)(void*),int flags){
    auto pid= clone(function,stack_mem(),flags,0);
    isOK(pid,"Clone Process Error");
    wait(nullptr);

}
void setupENV(){
    clearenv();
    setenv("TERM","xterm-256color",0);
    setenv("PATH","/bin/:/sbin/:/usr/sbin",0);
}
void setupChild(){
    map<string,string> c=parse("container_config.ini");
    const char* root = c["custom_root"].c_str();
    const char* cpuManage = c["cpu_manage"].c_str();
    const char* memory = c["memory"].c_str();
    const char* maxProcess = c["maxProcess"].c_str();
    const char* hostname = c["host_name"].c_str();
    WRITE(CGROUP_MEM,memory);
    WRITE(CGROUP_CPU,cpuManage);
    WRITE(concat(REQ_CGROUP,"/pid.max"),maxProcess);

    isOK(sethostname(hostname,strlen(hostname)),"hostname error");
    setupENV();






}
int childProcess(void *args){
    setupChild();
    return 0;
}
void makeCgroup(){
    WRITE(CGROUP_SUBTREE,"+cpu +memory +pids");
    mkdir(REQ_CGROUP,S_IRUSR | S_IWUSR);
    const char* pid = to_string(getpid()).c_str();
    // cout<<pid;
    WRITE(concat(REQ_CGROUP,"/cgroup.procs"),pid);

}
int main(){
    cout<<"Parent Pid :"<<getpid()<<endl;
    makeCgroup();
    cloneProcess();
    
}