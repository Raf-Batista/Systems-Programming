#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#define CONNECTION_SOCKET_NAME  "/tmp/data_sync_socket"
#define MAX_CONNECTIONS 32

/* We need to keep track of max number of fds. MAX_FDS + 1 is passed into select() */
int MAX_FDS = 0;

// Main FD sets. We will add, remove from this set
fd_set read_fds[MAX_CONNECTIONS];
fd_set write_fds[MAX_CONNECTIONS];

/* This is set where we will wait for an event to happen
* We will initialize this to read_fds and write_fds in main loop
* We don't want to modify read_fds and write_fds since select() sys call
* modifies the FDs in the set.
* */
fd_set tmp_read_fds[MAX_CONNECTIONS];
fd_set tmp_write_fds[MAX_CONNECTIONS];

// Operation Code
typedef enum {
    CREATE,
    UPDATE,
    DELETE
} OPCODE;

// Structure containing data to be synchronized
typedef struct _msg_body {
    char destination[16];
    char mask;
    char gateway_ip[16];
    char oif[32];
} msg_body_t;

// final message that needs to be synced from routing table manager process to all clients. Should have OP Code and body
typedef struct _sync_msg {
    OPCODE op_code;
    msg_body_t msg_body;
} sync_msg_t;

/* Function to add to main fd set */
void add_to_fd_set(int socket) {
    FD_SET(socket, read_fds);
    FD_SET(socket, write_fds);
    MAX_FDS += 1;
}

/* Function to remove from main fd set
 * if recv() returns 0, client closed connection
 */
void remove_from_fd_set(int socket) {
    FD_CLR(socket, read_fds);
    FD_CLR(socket, write_fds);
    MAX_FDS -= 1;
}

int main(int argc, char *argv[]) {
    /* Remove socket if program exited abruptly */
    unlink(CONNECTION_SOCKET_NAME);

    struct sockaddr_un socket_hint;
    int connection_socket;
    int ret;

    // create the socket
    printf("Creating Master Socket File Descriptor...\n");
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1) {
        perror("socket");
        return -1;
    }
    printf("Master socket file descriptor created...\n");

    // populate hint for bind

    /* Clear data structure */
    memset(&socket_hint, 0, sizeof(socket_hint));


    // bind the socket

    printf("Closing the master socket file descriptor...\n");
    close(connection_socket);

    return 0;
}