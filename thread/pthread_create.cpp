
#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

struct Person{
    int id;
    char name[23];
    int age;
};

struct Person p;

void* working( void* arg){
    printf("im a son, my id is %ld\n", pthread_self());
    for( int i = 0; i<9;++i){
        printf("child == i: = %d\n" ,i);
        if( i == 6){
            p.age = 12;
            strcpy(p.name, "tom");
            p.id = 100;
        }
    }
    return NULL;
}

int main(){
    
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);

    printf(" son is created, the id of son is %ld\n", tid);
    printf(" im a father, my id is %ld\n", pthread_self());

    for( int i = 0; i< 4;i++){    
        printf("i=%d\n", i);
    }
    sleep(3);
    printf("name:%s, age:%d, id: %d\n", p.name,p.age, p.id );
    printf("my resourses is called");
    return 0;
}
