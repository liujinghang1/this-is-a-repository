
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
struct Node{
    int number;
    struct Node* next;
};

pthread_cond_t cond;

pthread_mutex_t mutex;

struct Node* head = NULL;

void* producer(void* arg){
    while(1){
        pthread_mutex_lock(&mutex);

        struct Node* pnew = (struct Node*)malloc(sizeof(struct Node));

        pnew->number = rand()%1000;

        pnew->next = head;
        head = pnew;

        printf("+++producer,number = %d, tid = %ld\n",pnew->number,pthread_self());
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond);

        sleep(rand()%3);
    }
    return NULL;
}

void* consumer(void* arg){
    
    while(1){
        
        pthread_mutex_lock(&mutex);

        while(head == NULL){
            pthread_cond_wait(&cond, &mutex);
        }

        struct Node* pnode = head;
        printf("--conusmer:number:%d, tid = %ld\n",pnode->number,pthread_self());
        head = pnode->next;
        free(pnode);
        pthread_mutex_unlock(&mutex);

        sleep(rand()%3);
    }
    return NULL;
}

int main(){
    pthread_cond_init(&cond,NULL);
    pthread_mutex_init(&mutex,NULL);

    pthread_t ptid[5];
    pthread_t ctid[5];

    for( int i=0;i<5;++i){
        pthread_create(&ptid[i],NULL,producer,NULL);
    }
    for( int i=0;i<5;++i){
        pthread_create(&ctid[i],NULL,consumer,NULL);
    }

    for( int i=0;i<5;++i){
       pthread_join(ptid[i],NULL);

    }
    for(int i=0;i<5;++i){
        pthread_join(ctid[i],NULL);
    }

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;
}
