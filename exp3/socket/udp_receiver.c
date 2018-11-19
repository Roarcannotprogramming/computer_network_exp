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
    struct sockaddr_in recv_addr, clet_addr;
    int sin_len;
    char recv_buf[UDP_BUF_LENGTH];
    int sock_fd;
    int pkt_len;

    memset(&recv_addr, 0, sizeof(recv_addr));
    memset(&clet_addr, 0, sizeof(clet_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(60032);

    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("create socket error");
        exit(0);
    }

    if (bind(sock_fd, (struct sockaddr *) &recv_addr, sizeof(recv_addr)) == -1) {
        printf("bind socket error");
        exit(0);
    }

    while (1) {
        memset(recv_buf, 0, sizeof(recv_buf));
        sin_len = sizeof(clet_addr);
        pkt_len = recvfrom(sock_fd, recv_buf, UDP_BUF_LENGTH, 0, (
        struct sockaddr*)&clet_addr, sizeof(clet_addr));
        recv_buf[pkt_len] = '\0';
        printf("[UDP_RECEIVER] receive msg[%d bytes]\n", pkt_len);
        printf("\t%s\n", recv_buf);
    }

    close(sock_fd);
    return 0;
}

