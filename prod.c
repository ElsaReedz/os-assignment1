#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include<math.h>
#include <pthread.h>

#define NUM_THREADS 3
#define MAX_PRODUCT 20 
#define BUFFER_SIZE 20

typedef struct buf	
{		
	int rear;			
	int front;				
	int buffer[20];			
};
typedef struct pack
{
	int pid_self;
	double lambda;
};

int exp_random(double lambda)
{
	double pV = 0.0;
    while(1)
    {
        pV = (double)rand()/(double)RAND_MAX;
        if (pV != 1)
        {
            break;
        }
    }
    pV = (-1.0/lambda)*log(1-pV);
    return (int)(pV * 1000000);
}

sem_t *full;
sem_t *empty;
sem_t *s_mutex ;
pthread_t tid[NUM_THREADS];
void *ptr;


void *producer(void *param){
    struct pack* p = (struct pack*)param;
    double lambda = p->lambda;
    int idx = p->pid_self;
    do{
        int sleep_time = exp_random(lambda);
        usleep(sleep_time);
        int item = rand() % 10;
        struct buf *shm_ptr = ((struct buf *) ptr);
        sem_wait(empty);
        sem_wait(s_mutex);
        printf("Producing the data %d to buffer[%d] by id %d\n",item,shm_ptr->rear,idx);
        shm_ptr->buffer[shm_ptr->rear] = item;
        shm_ptr->rear = (shm_ptr->rear+1) % BUFFER_SIZE;
        sem_post(s_mutex);
        sem_post(full);
    }while(1);
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    struct buf shareMemory;
    memset(&shareMemory,0,sizeof(struct buf));   
    full = sem_open("full", O_CREAT,0666,0);
    empty = sem_open("empty",O_CREAT,0666,0);
    s_mutex = sem_open("mutex",O_CREAT,0666,0);

    sem_init(full,1,0);
    sem_init(empty,1,BUFFER_SIZE);
    sem_init(s_mutex,1,1);

    int shm_fd = shm_open("buffer",O_CREAT|O_RDWR,0666);
    ftruncate(shm_fd,sizeof(struct buf));
    ptr = mmap(0,sizeof(struct buf), PROT_WRITE, MAP_SHARED,shm_fd,0);
        pthread_t tid1,tid2,tid3;
    pthread_attr_t attr1,attr2,attr3;
    struct pack p1, p2, p3;
    p1.pid_self = 1;
    p2.pid_self = 2;
    p3.pid_self = 3;
    p1.lambda = p2.lambda = p3.lambda = atof(argv[1]);
    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);
    pthread_create(&tid1,&attr1,producer,(void*)&p1);
    pthread_create(&tid2,&attr2,producer,(void*)&p2);
    pthread_create(&tid3,&attr3,producer,(void*)&p3);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);

    return 0;
}
