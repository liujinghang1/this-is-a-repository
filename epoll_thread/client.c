#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    // 1. 创建用于通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 连接服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;     // ipv4
    addr.sin_port = htons(9999);   // 服务器监听的端口, 字节序应该是网络字节序
    inet_pton(AF_INET, "192.168.0.107", &addr.sin_addr.s_addr);
    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("connect");
        exit(0);
    }
	
	
    // 通信
    int num=0;
    char buf[1024] = {0};
    while(num <200)
    {
        
        sprintf(buf, "hello world: %d\n", num++);
	    printf("%s\n",buf);
        write(fd, buf, strlen(buf)+1);
        // 如果客户端没有发送数据, 默认阻塞
        read(fd, buf, sizeof(buf));
        printf("recv buf: %s\n", buf);
        usleep(100000);
    }

    // 释放资源
    close(fd); 

    return 0;
}

