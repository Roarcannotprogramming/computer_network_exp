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

#define MAXLINE 4096

int main(int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    char buff[4096];
    int n;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error");
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(60033);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error");
        exit(0);
    }

    if (listen(listenfd, 10) == -1) {
        printf("listen socket error");
        exit(0);
    }

    printf("---------waiting for request----------\n");

    while (1) {
        if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) == -1) {
            printf("accept socket error");
            continue;
        }

        n = (int)recv(connfd, buff, MAXLINE, 0);

        buff[n] = '\0';

        printf("recive from client: %s\n", buff);
        printf("please reply:\n");
        fgets(buff, 4096, stdin);

        if(send(connfd, buff, n, 0) < 0){
            printf("send error");
            close(connfd);
            exit(0);
        }

        close(connfd);
    }

    close(listenfd);

}