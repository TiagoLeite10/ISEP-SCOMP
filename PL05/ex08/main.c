#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>

#include "data_struct.h"
#include "generate_random_values.h"

#define NUM_THREADS 5

pthread_mutex_t mux;
pthread_cond_t cond;

int finished_threads = 0;
int grant_order = 0;

void *my_thread_func(void *arg) {
	
	pthread_t *response = (pthread_t*)malloc(sizeof(pthread_t));
	*response = pthread_self();

	data_struct *data = (data_struct*)arg;
	
	for (int i = data->start_pos; i <= data->end_pos; i++) {
		data->result_data[i] = data->data[i] * 10 + 2;
	}

	// Lock
	if (pthread_mutex_lock(&mux) != 0) {
		perror("Error to lock mutex!\n");
		exit(EXIT_FAILURE);
	}

	finished_threads++;
	while (finished_threads != NUM_THREADS || grant_order != data->print_order) {
		if (pthread_cond_broadcast(&cond) != 0) {
			perror("Error on broadcasting conditional mutex!\n");
			exit(EXIT_FAILURE);
		}

		if (pthread_cond_wait(&cond, &mux) != 0) {
			perror("Error on waiting for conditional wait!\n");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = data->start_pos; i <= data->end_pos; i++) {
		printf("result[%d] = %d\n", i, data->result_data[i]);
	}
	
	grant_order++;
	if (pthread_cond_broadcast(&cond) != 0) {
		perror("Error on broadcasting conditional mutex!\n");
		exit(EXIT_FAILURE);
	}

	// Unlock
	if (pthread_mutex_unlock(&mux) != 0) {
		perror("Error on unlocking mutex!\n");
		exit(EXIT_FAILURE);
	}

	pthread_exit((void*)response);

}

int main() {

	data_struct data_s[NUM_THREADS];
	int result_data[DATA_SIZE];
	int data[DATA_SIZE];
	generate_random_values(data, DATA_SIZE);

	pthread_t threads[NUM_THREADS];
	int partition = DATA_SIZE / NUM_THREADS;
	
	if (pthread_mutex_init(&mux, NULL) != 0) {
		perror("Error on mutex init!\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_cond_init(&cond, NULL) != 0) {
		perror("Error on cond init!\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		data_s[i].print_order = i;
		data_s[i].result_data = result_data;
		data_s[i].data = data;
		data_s[i].start_pos = i * partition;
		data_s[i].end_pos = (i * partition) + partition - 1;
		if (pthread_create(&threads[i], NULL, my_thread_func, (void*)&data_s[i]) != 0) {
			perror("Error in thread creation!\n");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		void *response = 0;
		if (pthread_join(threads[i], (void*)&response) != 0) {
			perror("Error in thread join!\n");
			exit(EXIT_FAILURE);
		}

		printf("Response: %lu\n", *((pthread_t*)response));
		free(response);
	}

	printf("Todas as threads terminaram a sua execução!\n");

	if (pthread_cond_destroy(&cond) != 0) {
		perror("Error in cond destroy!\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_mutex_destroy(&mux) != 0) {
		perror("Error on mutex detroy!\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

