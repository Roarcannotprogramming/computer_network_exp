//
// Created by roar on 18-11-17.
//

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

#define MAXLINECLI 4096

int main(int argc, char **argv) {
    char buffline[MAXLINECLI], recbuffline[MAXLINECLI];
    int sockfd, connfd;
    struct sockaddr_in cliaddr;
    int rec_len;

    if (argc != 2) {
        printf("usage: ./client <ipaddress>\n");
        exit(0);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error!");
        exit(0);
    }

    memset(&cliaddr, 0, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(60033);

    if (inet_pton(AF_INET, argv[1], &cliaddr.sin_addr) <= 0) {
        printf("inet_pton error for %s\n", argv[1]);
        exit(0);
    }

    if (connect(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0) {
        printf("connect error");
        exit(0);
    }

    printf("send msg to server: \n");
    fgets(buffline, 4096, stdin);
    if (send(sockfd, buffline, strlen(buffline), 0) < 0) {
        printf("send msg error");
        exit(0);
    }

    if ((rec_len = (int) recv(sockfd, recbuffline, MAXLINECLI, 0)) == -1) {
        perror("recv error");
        exit(1);
    }

    recbuffline[rec_len] = '\0';

    printf("recive form server: %s", buffline);

    close(sockfd);
    exit(0);

}
