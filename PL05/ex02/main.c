#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>

#include "data_struct.h"

#define NUM_ELEMENTS 5

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
	data_struct my_data[NUM_ELEMENTS];

	strcpy(my_data[0].name, "Aluno 1");
	my_data[0].grade = 10;
	
	strcpy(my_data[1].name, "Aluno 2");
	my_data[1].grade = 12;

	strcpy(my_data[2].name, "Aluno 3");
	my_data[2].grade = 14;

	strcpy(my_data[3].name, "Aluno 4");
	my_data[3].grade = 17;

	strcpy(my_data[4].name, "Aluno 5");
	my_data[4].grade = 20;

	for (int i = 0; i < NUM_ELEMENTS; i++) {
		my_data[i].number = i + 1;
	}

	pthread_mutex_init(&mux, NULL);

	for (int i = 0; i < NUM_ELEMENTS; i++) {
		if (pthread_create(&threads[i], NULL, my_thread_func, (void*)&my_data[i]) != 0) {
			printf("Error while creating threads!\n");
			exit(EXIT_FAILURE);
		}
	}

	printf("All threads created!\n");

	for (int i = 0; i < NUM_ELEMENTS; i++) {
		if (pthread_join(threads[i], NULL) != 0) {
			printf("Error in join!\n");
			exit(EXIT_FAILURE);
		}
	}

	printf("All threads terminated!\n");

	pthread_mutex_destroy(&mux);

	return 0;
}

