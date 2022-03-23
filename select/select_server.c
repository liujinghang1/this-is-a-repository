#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

int main()
{

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

        rdtemp = rdset;
        int num = select(maxfd+1, &rdtemp, NULL, NULL, NULL);

        if(FD_ISSET(lfd,&rdtemp)){
            
            struct sockaddr_in cliaddr;
            int clilen = sizeof(cliaddr);
            int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &clilen);

            FD_SET(cfd, &rdset);
            char ip[24] = {0};
           printf("the add of client:%s, port:%d\n",
           inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(cliaddr.sin_port));
            maxfd = cfd > maxfd ? cfd : maxfd;
        }
            int i;
            for( i = 0; i<maxfd+1;++i){
                if(i != lfd && FD_ISSET(i, &rdtemp)){
                    
                    char buf[1000] = {0};

                    int len = read(i, buf, sizeof(buf));
                    if(len == 0){
                        
                        printf(" client closed...\n");
                        FD_CLR(i, &rdset);
                        close(i);
                    }
                    else if(len > 0){
					printf("read buf = %s\n", buf);
					int i;
					for( i = 0;i<len;++i){
						buf[i] = toupper(buf[i]);
						}
					printf("after buf = %s\n", buf);
                        write(i, buf, strlen(buf)+1);
                    }
                    else{
                        perror("read");
                    }
                }
            }
        }
    
                return 0;
}







