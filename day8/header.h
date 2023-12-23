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