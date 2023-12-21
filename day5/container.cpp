#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

#define CGROUP_SUBTREE "/sys/fs/cgroup/cgroup.subtree_control"
#define REQ_CGROUP "/sys/fs/cgroup/cntr"

void WRITE(const char* path, const char* value) {
    int fd = open(path, O_WRONLY | O_APPEND); 
    if (fd == -1) {
        perror("Error in opening");
        exit(1);
    }

    ssize_t bytes = write(fd, value, strlen(value));  
    if (bytes == -1) {
        perror("Error in writing");
    }

    close(fd);
}

void makeCgroup() {
    WRITE(CGROUP_SUBTREE, "+cpu +memory +pids");  // Corrected subsystem names
    mkdir(REQ_CGROUP, S_IRUSR | S_IWUSR | S_IXUSR);  // Added execute permission
}

int main() {
    cout << "parent pid: " << getpid() << endl;
    makeCgroup();

    return 0;
}