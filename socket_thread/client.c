#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>


int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1){
        perror("socket");
        exit(0);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1111);
    inet_pton(AF_INET, "192.168.0.106",&addr.sin_addr.s_addr);
    
    int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
    if(ret == -1){
        perror("connect");
        exit(0);
    }

    int number = 0;
    while(1){
        char buf[1024];
       sprintf(buf,"hello server...%d\n", number++);
       write(fd, buf, strlen(buf)+1);

       memset(buf, 0, sizeof(buf));
       int len = read(fd, buf, sizeof(buf));
       if(len > 0){
            printf("server say :%s\n",buf);}
       else if( len == 0){
            printf("sercer leave ....\n");
            break;
       }
       else{
            perror("read");
            break;
       }
       sleep(1);
       }

       close(fd);

       return 0;

    
}
