#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define UDP_BUF_LENGTH 4096

int main(int argc, char **argv) {
    struct sockaddr_in recv_addr;
    int sock_fd, n;
    int counter = 0;
    int total = 20;
    char send_buf[UDP_BUF_LENGTH];

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("create socket error");
        exit(0);
    }
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr =htonl(INADDR_ANY);
    recv_addr.sin_port =htons(60032);

    memset(send_buf, 0, sizeof(send_buf));
    printf("sending data packet begins: \n");
    n = sprintf(send_buf, "%d", total);
    sendto(sock_fd, send_buf, (size_t)n, 0, (struct sockaddr*)&recv_addr, sizeof(recv_addr));
    sleep(1);

    while (1) {
        memset(send_buf, 0, sizeof(send_buf));
        printf("sending data packet with #: %d\n", counter);
        n = sprintf(send_buf, "data packet with #: %d", counter);
        sendto(sock_fd, send_buf, (size_t)n, 0, (struct sockaddr*)&recv_addr, sizeof(recv_addr));
        counter++;
        if (counter ==total) break;
        sleep(1);
    }

    close(sock_fd);
    return 0;
}

