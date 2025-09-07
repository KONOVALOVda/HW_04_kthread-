#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2
#define ITERATIONS 1000

int shared_data = 0;
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void *reader_fn(void *arg)
{
	int id = (long)arg;
	for (int i = 0; i < ITERATIONS; i++) {
		pthread_rwlock_rdlock(&rwlock);
		printf("Reader %d reads %d\n", id, shared_data);
		pthread_rwlock_unlock(&rwlock);
		usleep(1000);
	}
	return NULL;
}

void *writer_fn(void *arg)
{
	int id = (long)arg;
	for (int i = 0; i < ITERATIONS; i++) {
		pthread_rwlock_wrlock(&rwlock);
		shared_data++;
		printf("Writer %d writes %d\n", id, shared_data);
		pthread_rwlock_unlock(&rwlock);
		usleep(1000);
	}
	return NULL;
}

int main()
{
	pthread_t readers[NUM_READERS];
	pthread_t writers[NUM_WRITERS];

	for (long i = 0; i < NUM_READERS; i++)
		pthread_create(&readers[i], NULL, reader_fn, (void *)i);

	for (long i = 0; i < NUM_WRITERS; i++)
		pthread_create(&writers[i], NULL, writer_fn, (void *)i);

	for (int i = 0; i < NUM_READERS; i++)
		pthread_join(readers[i], NULL);

	for (int i = 0; i < NUM_WRITERS; i++)
		pthread_join(writers[i], NULL);

	return 0;
}
