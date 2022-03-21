#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main()
{
    //AF_INET IPV4, SOCKET_STREAM TCP
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1){
        perror("socket");
        exit(0);
    }


    //SOCKADDR_IN 结构体
    //htons大端端口
    //INADDR_ANY本机的所有IP，一般用于本地绑定，宏的值为0不用转大小端
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);
    addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(lfd, (struct sockaddr*)&addr,sizeof(addr));
    if(ret == -1){
        perror("bind");
        exit(0);
    }

    //listen
    //128同时处理最大的连接请求
    ret = listen(lfd, 128);
    if(ret == -1){
        perror("listen");
        exit(0);
    }

    //accept
    //存放客户端地址
    struct sockaddr_in cliaddr;
    int clilen = sizeof(cliaddr);
    int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);
    if(cfd == -1){
        perror("accept");
        exit(0);
    }

    char ip[24] = {0};
    printf("the add of client:%s, port:%d\n",
           inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(cliaddr.sin_port));

    while(1){
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        int len = read(cfd, buf, sizeof(buf));
        if(len > 0){
            printf("client say %s\n", buf);
            write(cfd, buf, len);
        }
        else if(len == 0){
            printf("client down\n");
            break;
        }
        else{
            perror("read");
            break;
        }
    }

    close(cfd);
    close(lfd);

    return 0;

}



















