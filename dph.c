#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// the number of philosophers
#define NUMBER 5

// the maximum time (in seconds) to sleep
#define MAX_SLEEP_TIME	3

// the state of each philosopher (THINKING, HUNGRY, EATING)
enum {THINKING, HUNGRY, EATING} state[NUMBER];

// the thread id of each philosopher (0 .. NUMBER - 1)
int thread_id[NUMBER];

// condition variables and associated mutex lock
pthread_cond_t		cond_vars[NUMBER];
pthread_mutex_t 	mutex_lock;

void *philosopher(void *param);
pthread_t tid[NUMBER];

void init()
{
	int i;
	
	// Initialize the condition variables and mutex lock
	for (i = 0; i < NUMBER; i++) 
	{
		state[i] = THINKING;
		thread_id[i] = i;
		pthread_cond_init(&cond_vars[i],NULL);
	}

	pthread_mutex_init(&mutex_lock, NULL);
	
	// Create five philosophers
	for (i = 0; i < NUMBER; i++) 
		pthread_create(&tid[i], 0, philosopher, (void *)&thread_id[i]);
}

void test(int i)
{
	if ( (state[(i + NUMBER - 1) % NUMBER] != EATING) && (state[i] == HUNGRY) && (state[(i + 1) % NUMBER] != EATING) ) 
	{
		state[i] = EATING;
		printf("Philosopher %d is eating\n", i);
		pthread_cond_signal(&cond_vars[i]);   // Resumes one of philosophers who is waiting to eat (If no philosopher is waiting to eat, nothing happens)
	}
}

void pickup_forks(int number)
{
	pthread_mutex_lock(&mutex_lock);
	
	state[number] = HUNGRY;
	
	test(number);	// If I'm hungry, and my left and right neighbors aren't eating, then I will eat.
	while (state[number] != EATING)
		pthread_cond_wait(&cond_vars[number], &mutex_lock);	  // If I cannot eat at the moment, I will be suspended
	
	pthread_mutex_unlock(&mutex_lock);
}

void return_forks(int number)
{
	pthread_mutex_lock(&mutex_lock);
	
	state[number] = THINKING;
	printf("Philosopher %d is thinking\n", number);
	
	// Check if neighbors are waiting to eat
	test((number + NUMBER - 1) % NUMBER);
	test((number + 1) % NUMBER);

	pthread_mutex_unlock(&mutex_lock);
}

void *philosopher(void *param)
{
	int number = *(int *)param;
	int sleep_time = (rand() % 3) + 1;	// Sleep for a random period between 1-3 seconds
	//int loop = 0;	// For testing, each philosopher thinks three times and eats three times

	//for(loop; loop < 3; loop++) 
	while(1)
	{
		sleep(sleep_time);
		pickup_forks(number);	// Philosopher 'number' wishes to eat

		sleep(sleep_time);
		return_forks(number);	// Philosopher 'number' finishes eating
	}
}

int main()
{
	init();
	
	int i;
	for (i = 0; i < NUMBER; i++)
		pthread_join(tid[i],NULL);
	return 0;
}
