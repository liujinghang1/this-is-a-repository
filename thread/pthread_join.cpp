
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

void* working( void* arg){
 
     printf("im a son, my id is %ld\n", pthread_self());
    for( int i = 0; i<9;++i){
        printf("child == i: = %d\n" ,i);
        if( i == 6){
            struct Person* p = (struct Person*)arg;
            p->age = 12;
            strcpy(p->name, "tom");
            p->id = 100;
            pthread_exit(p);
        }
    }

    return NULL;
}

int main(){
    
    pthread_t tid;
    struct Person p;
    pthread_create(&tid, NULL, working, &p);

    printf(" son is created, the id of son is %ld\n", tid);
    printf(" im a father, my id is %ld\n", pthread_self());

    for( int i = 0; i< 4;i++){    
        printf("i=%d\n", i);
    }
    
    void* ptr;
    pthread_join(tid, &ptr);
    struct Person* pp = (struct Person*)ptr; 
    printf("name:%s, age:%d, id: %d\n", p.name,p.age, p.id );
    printf("my resourses is called\n");

    printf("------name:%s, age:%d, id:%d\n-------",pp->name,pp->age,pp->id);
    return 0;
}
