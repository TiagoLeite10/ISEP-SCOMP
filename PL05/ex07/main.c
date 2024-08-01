#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_KEYS 8000
#define NUMBERS_BY_KEY 5
#define POSSIBLE_NUMBERS 49
#define NUMBER_THREADS 16
#define NUMBERS_ANALYZE (NUMBER_OF_KEYS / NUMBER_THREADS)

int array[NUMBER_OF_KEYS][NUMBERS_BY_KEY];
int statistics[POSSIBLE_NUMBERS];
pthread_mutex_t mutexes[POSSIBLE_NUMBERS];

void* count_keys(void *arg) {
	int i, j;
	int thread_num = *((int*) arg);
	int start_index = thread_num * NUMBERS_ANALYZE;
	int end_index = start_index + NUMBERS_ANALYZE;

	for (i = start_index; i < end_index; i++) {
		for (j = 0; j < NUMBERS_BY_KEY; j++) {
			int num = array[i][j];
			pthread_mutex_lock(&mutexes[num - 1]);			
			statistics[num - 1]++;
			pthread_mutex_unlock(&mutexes[num - 1]);
		}
	}

	pthread_exit(NULL);
}

int main() {
	pthread_t threads[NUMBER_THREADS];
	int thread_num[NUMBER_THREADS];
	int i, j;
	srand(time(NULL));

	for (i = 0; i < NUMBER_OF_KEYS; i++) {
		for (j = 0; j < NUMBERS_BY_KEY; j++) {
			array[i][j] = (rand() % 49) + 1;
		}
	}

	/* Inicializar mutexes */
	for (int i = 0; i < POSSIBLE_NUMBERS; i++) {
        pthread_mutex_init(&mutexes[i], NULL);
    }

	for (i = 0; i < NUMBER_THREADS; i++) {
		thread_num[i] = i;

		if (pthread_create(&threads[i], NULL, count_keys, &thread_num[i]) != 0) {
 			perror("Error creating thread!");
 			exit(EXIT_FAILURE);
 		}
	}

	for (i = 0; i < NUMBER_THREADS; i++) {
		if (pthread_join(threads[i], NULL) != 0) {
			perror("Error joining thread!");
 			exit(EXIT_FAILURE);
		}
	}

	printf("Statistical values\n");
	
	for (i = 0; i < POSSIBLE_NUMBERS; i++) {
		printf("Key %d: %d times\n", i + 1, statistics[i]);
	}
	
	/* Destruir mutexes */
	for (i = 0; i < POSSIBLE_NUMBERS; i++) {
        pthread_mutex_destroy(&mutexes[i]);
    }	

	return 0;
}