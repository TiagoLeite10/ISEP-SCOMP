#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 1000
#define NUMBER_THREADS 10
#define SEARCH_SIZE (ARRAY_SIZE / NUMBER_THREADS)
#define NUMBER 150

int array[ARRAY_SIZE];

void* search_number(void *arg) {
	int thread_num = *((int*)arg);
	int start_index = (thread_num - 1) * SEARCH_SIZE;
	int end_index = start_index + SEARCH_SIZE;

	for (int i = start_index; i < end_index; i++) {
		if (array[i] == NUMBER) {
			int* thread_result = malloc(sizeof(int));
            *thread_result = thread_num;           

			printf("Number %d found at position %d\n", NUMBER, i);
			pthread_exit((void*) thread_result);
		}
	}

	pthread_exit(NULL);
}

int main() {
	pthread_t threads[NUMBER_THREADS];
	int thread_num[NUMBER_THREADS];
	int i;

	for (i = 1; i <= ARRAY_SIZE; i++) {
		array[i] = i;
	}

	for (i = 0; i < NUMBER_THREADS; i++) {
		thread_num[i] = i + 1;

		if (pthread_create(&threads[i], NULL, search_number, (void*) &thread_num[i]) != 0) {
 			perror("Error creating thread!");
 			exit(EXIT_FAILURE);
 		}
	}

	int* found_thread_num = NULL;

	for (i = 0; i < NUMBER_THREADS; i++) {
        void* thread_result = NULL;

		if (pthread_join(threads[i], &thread_result) != 0) {
			perror("Error joining thread!");
 			exit(EXIT_FAILURE);
		}

		if (thread_result != NULL) {
			found_thread_num = (int*) thread_result;
			printf("Number %d found by thread %d\n", NUMBER, *found_thread_num);
			break;
		}
	}

	if (found_thread_num == NULL) {	
		printf("Number not found\n");
	}

	return 0;
}