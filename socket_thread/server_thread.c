#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<pthread.h>

struct SockInfo{
    int fd;
    pthread_t tid;
    struct sockaddr_in addr;
};

struct SockInfo infos[128];

void* working(void* arg){
    while(1){

        struct SockInfo* info = (struct SockInfo*)arg;

        char buf[1024];
        int ret = read(info->fd, buf,sizeof(buf));
        if(ret = 0){
            printf("client is closed....\n");
            info->fd = -1;
            break;
        }
        else if( ret == -1){
            printf(" data recieved failed....\n");
            info->fd = -1;
            break;
        }
        else{
            write(info->fd, buf, strlen(buf)+1);
        }
        
    }
    return NULL;
}
int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if( fd == -1){
        perror("socket");
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1111);
    addr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(fd, (struct sockaddr*)&addr,sizeof(addr));
    
    if(ret == -1){
        perror("bind");
        exit(0);
    }

    ret = listen(fd, 100);
    if(ret == -1){
        perror("listen");
        exit(0);
    }
    int len = sizeof(struct sockaddr);
    int max = sizeof(infos)/sizeof(infos[0]);
    int i;
    for(  i =0; i<max;++i){
        bzero(&infos[i],sizeof(infos[i]));
        infos[i].fd = -1;
        infos[i].tid = -1;
    }

    while(1){
        
        struct SockInfo* pinfo;
        int i;
        for( i =0;i<max;++i){
            if(infos[i].fd == -1){
                pinfo = &infos[i];
                break;
            }
            if(i==max-1){
                sleep(1);
                i--;
            }
        }

        int connfd = accept(fd, (struct sockaddr*)&pinfo->addr,&len);
        printf("parent thread, connfd:%d\n", connfd);
        char ip[24] = {0};
        printf("the ip of client is %s, port : %d\n",
               inet_ntop(AF_INET,&(pinfo->addr).sin_addr.s_addr,ip,sizeof(ip)),
               ntohs((pinfo->addr).sin_port));
        if(connfd == -1){
            perror("accept");
            exit(0);
        }
        pinfo->fd = connfd;
        pthread_create(&pinfo->tid,NULL,working,pinfo);
        pthread_detach(pinfo->tid);
    }
    close(fd);

    return 0;
}

