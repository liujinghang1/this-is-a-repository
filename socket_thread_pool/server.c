#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "thread_pool.h"

struct SockInfo
{
    int fd;                      // 通信
    pthread_t tid;               // 线程ID
    struct sockaddr_in addr;     // 地址信息
};


typedef struct PoolInfo{
	ThreadPool* p;
	int fd;
	}PoolInfo;
	

void working(void* arg);
void acceptConn(void* arg);

int main()
{
    // 1. 创建用于监听的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        perror("socket");
        exit(0);
    }

    // 2. 绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;          // ipv4
    addr.sin_port = htons(8989);        // 字节序应该是网络字节序
    addr.sin_addr.s_addr =  INADDR_ANY; // == 0, 获取IP的操作交给了内核
    int ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1)
    {
        perror("bind");
        exit(0);
    }

    // 3.设置监听
    ret = listen(fd, 100);
    if(ret == -1)
    {
        perror("listen");
        exit(0);
    }

    // 4. 等待, 接受连接请求
    

	
    ThreadPool* pool = threadPoolCreate(3, 8, 100);
    PoolInfo* info = (PoolInfo*)malloc(sizeof(PoolInfo));
    info->p = pool;
    info->fd = fd;
    threadPoolAdd(pool, acceptConn,info);
    pthread_exit(NULL);
    return 0;
}

void acceptConn(void* arg){
    // 父进程监听, 子进程通信
    PoolInfo* poolInfo = (PoolInfo*)arg;
    int len = sizeof(struct sockaddr_in);
    while(1)
    {
        // 创建子线程
         struct SockInfo* pinfo;
         pinfo = (struct SockInfo*)malloc(sizeof(struct SockInfo));  
	   pinfo->fd = accept(poolInfo->fd, (struct sockaddr*)&pinfo->addr, &len);
        if(pinfo->fd == -1)
        {
            perror("accept");
            exit(0);
        }
		threadPoolAdd(poolInfo-> p, working,pinfo);          
    }

    // 释放资源
    close(poolInfo->fd);  // 监听

    
}


void working(void* arg)
{
    struct SockInfo* info = (struct SockInfo*)arg;
    char ip[32];
    printf("the ip of client is %s, port is %d\n",
    		inet_ntop(AF_INET, &info->addr.sin_addr.s_addr,ip,sizeof(ip)),
    		ntohs(info->addr.sin_port));
    while(1)
    {
        
        // 接收数据
        char buf[1024];
        int len = recv(info->fd, buf, sizeof(buf),0);
        if(len > 0)
        {
            printf("client say:%s...\n",buf);
            send(info->fd,buf,len,0);
            
        }
        else if(len == 0)
        {
            printf("接收数据失败...\n");  
            break;
        }
        else
        {
          perror("recv");
          break;  
        }
    }
    close(info->fd);
}
