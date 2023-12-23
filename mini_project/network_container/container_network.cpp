#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

void setupNetworkNamespace() {
    // Create a new network namespace
    int netns = unshare(CLONE_NEWNET);
    if (netns == -1) {
        perror("unshare(CLONE_NEWNET)");
        exit(EXIT_FAILURE);
    }

    // Create a veth pair (virtual Ethernet devices)
    const char* veth1 = "veth1";
    const char* veth2 = "veth2";

    if (system(("ip link add " + string(veth1) + " type veth peer name " + string(veth2)).c_str()) == -1) {
        perror("ip link add");
        exit(EXIT_FAILURE);
    }

    // Move veth2 to the network namespace
    if (system(("ip link set " + string(veth2) + " netns $pid").c_str()) == -1) {
        perror("ip link set");
        exit(EXIT_FAILURE);
    }

    // Configure IP addresses for veth1 in the original namespace
    if (system(("ip addr add 192.168.1.1/24 dev " + string(veth1)).c_str()) == -1) {
        perror("ip addr add");
        exit(EXIT_FAILURE);
    }

    // Bring up veth1
    if (system(("ip link set " + string(veth1) + " up").c_str()) == -1) {
        perror("ip link set up");
        exit(EXIT_FAILURE);
    }

    // Set up NAT for internet access in the network namespace
    if (system("iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE") == -1) {
        perror("iptables");
        exit(EXIT_FAILURE);
    }

    cout << "Container network namespace set up successfully." << endl;
}

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process (container)
        setupNetworkNamespace();
        execlp("/bin/bash", "/bin/bash", NULL);

        // If execlp fails
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        waitpid(pid, NULL, 0);
        cout << "Parent process exiting." << endl;
    }

    return EXIT_SUCCESS;
}
