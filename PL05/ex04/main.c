#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>

#include "data_struct.h"

#define NUM_THREADS 5

pthread_mutex_t mux;

void *my_thread_func(void *arg) {
	data_struct *data = (data_struct*)arg;
	
	pthread_mutex_lock(&mux);

	printf("Thread número: %lu\n", pthread_self());
	printf("Number: %d\nNome: %s\nNota: %d\n", data->number, data->name, data->grade);
	printf("-------------------------------------\n");
	
	pthread_mutex_unlock(&mux);

	pthread_exit((void*)NULL);
}

int main() {
	
	pthread_t threads[NUM_ELEMENTS];

	return 0;
}

