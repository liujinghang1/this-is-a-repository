#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if( lfd == -1){
        perror("socket error");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9999);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int ret = bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(ret == -1){
        perror("bind error");
        exit(1);
    }

    ret = listen(lfd,64);
    if(ret == -1){
        perror("listen error");
        exit(1);
    }


    int epfd = epoll_create(100);
    if(epfd == -1){
        perror("epoll_create");
        exit(0);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN|EPOLLET;
    ev.data.fd = lfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if(ret == -1){
    perror("epoll_ctl");
    exit(0);
    }

    struct epoll_event evs[1024];
    int size = sizeof(evs)/sizeof(struct epoll_event);
 
    while(1){
    
            int num = epoll_wait(epfd, evs, size, -1);
            printf("===== fd num:%d\n=========", num);

            int i;
            for(i = 0;i<num;++i){
                
                int curfd = evs[i].data.fd;

                if(curfd == lfd){
                
                	int cfd = accept(curfd, NULL, NULL);

                	int flag = fcntl(cfd, F_GETFL);
                	flag |= O_NONBLOCK;
                	fcntl(cfd, F_SETFL, flag);

                	ev.events = EPOLLIN | EPOLLET;
                	ev.data.fd = cfd;
                	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                	if(ret == -1){
                    	perror("epoll_ctl_accept");
                    	exit(0);
                	}
                }
                else{

                    char buf[5];
                    memset(buf, 0, sizeof(buf));
                    while(1){
                        int len = recv(curfd, buf, sizeof(buf), 0);
                        
                        if(len == -1){

                            if(errno == EAGAIN){
                                
                                printf("data is read down....\n");
                                break;

                            }
                            else{
                                    perror("recv");
                                    exit(0);
                            }
                        }
                        else if(len==0){
                        
                            printf(" client is leave...\n");
                            epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                            close(curfd);
                            break;
                        }
                        
                        	printf("read buf = %s\n", buf);
					int i;
					  for(i = 0;i<len;++i){
					  	buf[i] = toupper(buf[i]);
					  	}
                            write(STDOUT_FILENO, buf, len);
                            printf("\n");
                            ret = send( curfd, buf, strlen(buf)+1, 0);
                            if(ret == -1){
                            	perror("send error");
                            	exit(1);
                            	}

                        
                        
            
                    }


                }
    }
}
}









