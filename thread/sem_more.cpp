
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>
struct Node{
    int number;
    struct Node* next;
};

sem_t psem;
sem_t csem;

pthread_mutex_t mutex;

struct Node* head = NULL;

void* producer(void* arg){
    while(1){
        sem_wait(&psem);
        pthread_mutex_lock(&mutex);
        struct Node* pnew = (struct Node*)malloc(sizeof(struct Node));

        pnew->number = rand()%1000;

        pnew->next = head;
        head = pnew;

        printf("+++producer,number = %d, tid = %ld\n",pnew->number,pthread_self());
      pthread_mutex_unlock(&mutex);
        sem_post(&csem);
        sleep(rand()%3);
    }
    return NULL;
}

void* consumer(void* arg){
    
    while(1){
        
        sem_wait(&csem);
        pthread_mutex_lock(&mutex);
        struct Node* pnode = head;
        printf("--conusmer:number:%d, tid = %ld\n",pnode->number,pthread_self());
        head = pnode->next;
        free(pnode);
        
        pthread_mutex_unlock(&mutex);
        sem_post(&psem);
        sleep(rand()%3);
    }
    return NULL;
}

int main(){
    
    sem_init(&psem,0,5);
    sem_init(&csem,0,0);

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

    sem_destroy(&psem);
    sem_destroy(&csem);

    return 0;
}
