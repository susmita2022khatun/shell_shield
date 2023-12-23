#include "parser.h"
#include <sched.h>
#include <cstdlib>
#include <sys/mount.h>
#include <fstream>
#include <ctime>
#include "header.h"

using namespace std;

ofstream logFile; // Declare a global file stream for logging

// Function to log messages to a file
void logMessage(const char *message) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    logFile << "[" << buffer << "] " << message << endl;
}

int isOK(int status, const char *msg) {
    if (status == -1) {
        perror(msg);
        logMessage(msg); // Log the error
        exit(EXIT_FAILURE);
    }
    return status;
}
int run(const char* name){
    char* args[] = {(char*)name,(char*)0};
    return execvp(name,args );
}

void WRITE(const char *path, const char *value) {
    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd == -1) {
        logMessage("Error in opening");
        exit(1);
    }
    ssize_t bytes = write(fd, value, strlen(value));
    if (bytes == -1) {
        logMessage("Error in writing");
        exit(1);
    }
    close(fd);
}

char *stack_mem() {
    const int stacksize = 65 * 1024;
    auto *stack = new (nothrow) char[stacksize];
    if (stack == nullptr) {
        logMessage("Can't allocate memory");
        exit(EXIT_FAILURE);
    }
    return stack + stacksize;
}

void cloneProcess(int (*function)(void *), int flags) {
    auto pid = clone(function, stack_mem(), flags, 0);
    isOK(pid, "Clone Process Error");
    wait(nullptr);
}

void setupENV() {
    clearenv();
    setenv("TERM", "xterm-256color", 0);
    setenv("PATH", "/bin/:/sbin/:/usr/sbin", 0);
}

void setupRoot(const char *folder) {
    isOK(chroot(folder), " cant set root:");
    isOK(chdir("/"), "chdir: ");
}

void setupjail() {
    map<string, string> c = parse("container_config.ini");
    const char *root = c["custom_root"].c_str();
    const char *cpuManage = c["cpu_manage"].c_str();
    const char *memory = c["memory"].c_str();
    const char *maxProcess = c["maxProcess"].c_str();
    const char *hostname = c["host_name"].c_str();
    WRITE(CGROUP_MEM, memory);
    WRITE(CGROUP_CPU, cpuManage);

    logMessage("Setting up jail environment"); // Log setup information

    isOK(sethostname(hostname, strlen(hostname)), "hostname error");
    setupENV();
    setupRoot("./root");
}

int jail(void *args) {
    setupjail();

    mount("proc", "/proc", "proc", 0, 0);
    pid_t shellPid = fork();
    isOK(shellPid, "can't create fork");
    if (shellPid == 0) {
        logMessage("Executing /bin/bash"); // Log command execution
        run("/bin/bash");
        exit(0);
    }

    // wait till all child processes
    while (wait(nullptr) > 0);

    // zombie check out
    // unmount the procfs when all processes are finished
    umount("proc");
    logMessage("Container execution completed"); // Log container completion
    return EXIT_SUCCESS;
}

void makeCgroup() {
    WRITE(CGROUP_SUBTREE, "+cpu +memory +pids");
    mkdir(REQ_CGROUP, S_IRUSR | S_IWUSR);
    const char *pid = to_string(getpid()).c_str();
    WRITE(concat(REQ_CGROUP, "/cgroup.procs"), pid);
}

int main() {
    logFile.open("container_log.txt", ios::out | ios::app); // Open log file

    logMessage(("Parent Pid: " + to_string(getpid())).c_str());

    makeCgroup();
    cloneProcess(jail, CLONE_NEWPID | CLONE_NEWUTS | SIGCHLD);

    logFile.close(); // Close log file
    return EXIT_SUCCESS;
}
