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
sem_t *s_mutex ;//mutex for struct
pthread_t tid[NUM_THREADS];
void *ptr;


void *producer(void *param){
    int idx = * (int *) param;
    double lambda = (* (int *) param) * 1.0;
    do{
        int sleep_time = exp_random(lambda);
        usleep(sleep_time);
        int item = rand() % 10;
        struct buf *shm_ptr = ((struct buf *) ptr);
        sem_wait(empty);
        sem_wait(s_mutex);
        printf("Producing the data %d to buffer[%d] by id %ld\n",item,shm_ptr->rear,tid[idx]);
        shm_ptr->buffer[shm_ptr->rear] = item;
        shm_ptr->rear = (shm_ptr->rear+1) % BUFFER_SIZE;
        sem_post(s_mutex);//Unlock the binary-mutex
        sem_post(full);// Add a full buffer
    }while(1);
    pthread_exit(0);
}

int main()
{
    struct buf shareMemory;
    memset(&shareMemory,0,sizeof(struct buf));
    //初始化内存空间，把他们全设置成为0
    full = sem_open("full", O_CREAT,0666,0);
    //sem_t *sem_open(const char *name, int oflag,/*mode_t mode, unsigned int value*/);
    //name:信号量的名称.  oflag: 使用O_CREAT 如果信号量已经存在，那么我什么也不做，函数也不会报错
    //mode: 表示谁可以访问信号量。 value：指定信号量的初始值。
    empty = sem_open("empty",O_CREAT,0666,0);
    s_mutex = sem_open("mutex",O_CREAT,0666,0);

    sem_init(full,1,0);
    //extern int sem_init __P ((sem_t *__sem, int __pshared, unsigned int __value));　　
    // sem为指向信号量结构的一个指针；
    // pshared不为０时此信号量在进程间共享，否则只能为当前进程的所有线程共享；
    // value给出了信号量的初始值。
    sem_init(empty,1,BUFFER_SIZE);
    sem_init(s_mutex,1,1);

    int shm_fd = shm_open("buffer",O_CREAT|O_RDWR,0666);
    ftruncate(shm_fd,sizeof(struct buf));
    //开辟出空间，fd就是我们常说的文件句柄
    ptr = mmap(0,sizeof(struct buf), PROT_WRITE, MAP_SHARED,shm_fd,0);
    //void* mmap(void* start,size_t length,int prot,int flags,int fd,off_t offset);
    //start:想要映射的的内存的起始地质，通常设置为NULL， length: 多大的部分映射到内存
    //prot 映射区域的保护方式，PROT_EXEC 映射区域可被执行 PROT_READ 映射区域可被读取PROT_WRITE 映射区域可被写入 PROT_NONE 映射区域不能存取
    //falgs map_shared 对映射区域的写入数据会复制回文件内，而且允许其他映射该文件的进程共享。
    //fd 要映射到内存中的文件描述符
    //offset 偏移量                        成功则返回内存起始地质

    pthread_t tid1,tid2,tid3;/*The thread identifier*/
    pthread_attr_t attr1,attr2,attr3;/*Set of thread attributes*/
    int lambda_1 = 1;
    int lambda_2 = 2;
    int lambda_3 = 3;
    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);
    pthread_create(&tid1,&attr1,producer,&lambda_1);
    pthread_create(&tid2,&attr2,producer,&lambda_2);
    pthread_create(&tid3,&attr3,producer,&lambda_3);
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);

    return 0;
}
