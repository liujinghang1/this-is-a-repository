#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<sys/select.h>


pthread_mutex_t mutex;
typedef struct fdinfo{
	int fd;
	int *maxfd;
	fd_set* rdset;
	}FDInfo;

void* acceptConn(void* arg){

	printf("son thread id is %ld\n",pthread_self());
	FDInfo* info = (FDInfo*)arg;
	
            struct sockaddr_in cliaddr;
            int clilen = sizeof(cliaddr);
            int cfd = accept(info->fd, (struct sockaddr*)&cliaddr, &clilen);
	   pthread_mutex_lock(&mutex);
            FD_SET(cfd, info->rdset);
            char ip[24] = {0};
           printf("the add of client:%s, port:%d\n",
           inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(cliaddr.sin_port));
            *info->maxfd = cfd > *info->maxfd ? cfd : *info->maxfd;
	   pthread_mutex_unlock(&mutex);
		free(info);
		return NULL;
	}

void* communication(void* arg){
	
	printf("son thread id is %ld\n",pthread_self());
        	
	FDInfo* info = (FDInfo*)arg;
	
                    char buf[1000] = {0};

                    int len = read(info->fd, buf, sizeof(buf));
                    if(len == 0){
                        
                        printf(" client closed...\n");
                        pthread_mutex_lock(&mutex);
                        FD_CLR(info->fd, info->rdset);
                        pthread_mutex_unlock(&mutex);
                        close(info->fd);
                        free(info);
                        return NULL;
                    }
                    else if(len > 0){
					printf("read buf = %s\n", buf);
					int i;
					for( i = 0;i<len;++i){
						buf[i] = toupper(buf[i]);
						}
					printf("after buf = %s\n", buf);
                        write(info->fd, buf, strlen(buf)+1);
                    }
                    else{
                        perror("read");
                        free(info);
                        return NULL;
                    }
                    free(info);
                    return NULL;
                }
	
int main()
{
	pthread_mutex_init(&mutex, NULL);
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1){
        perror("socket");
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(999);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(lfd, (struct sockaddr*)&addr, sizeof(addr));

    listen(lfd,128);

    int maxfd = lfd;
    fd_set rdset;
    fd_set rdtemp;

    FD_ZERO(&rdset);
    FD_SET(lfd, &rdset);

    while(1){
	   pthread_mutex_lock(&mutex);
        rdtemp = rdset;
	   pthread_mutex_unlock(&mutex);
        int num = select(maxfd+1, &rdtemp, NULL, NULL, NULL);

        if(FD_ISSET(lfd,&rdtemp)){
             
        	pthread_t tid;
        	FDInfo* info = (FDInfo*)malloc(sizeof(FDInfo));
        	info->fd = lfd;
        	info->maxfd = &maxfd;
        	info->rdset = &rdset;
        	pthread_create(&tid, NULL, acceptConn,info);
        	pthread_detach(tid);
        }
            int i;  
            for( i = 0; i<maxfd+1;++i){
                if(i != lfd && FD_ISSET(i, &rdtemp)){
                    
        	pthread_t tid;
        	FDInfo* info = (FDInfo*)malloc(sizeof(FDInfo));
        	info->fd = i;
        	info->rdset = &rdset;
        	pthread_create(&tid, NULL, communication, info);
        	pthread_detach(tid);

            }
        }
    }
    pthread_mutex_destroy(&mutex);
                return 0;
}







