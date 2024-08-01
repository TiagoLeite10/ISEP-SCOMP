#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>

#define ARRAY_SIZE 1000
#define NUMBER_THREADS 3

double balance_array[ARRAY_SIZE];
double lowest_balance = 0;
double highest_balance = 0;
double average_balance = 0;

void* search_lowest_balance(void *arg) {
    double min_balance = balance_array[0];

    for (int i = 1; i < ARRAY_SIZE; i++) {
        if (balance_array[i] < min_balance) {
            min_balance = balance_array[i];
        }
    }

    lowest_balance = min_balance;
    pthread_exit(NULL);
}

void* search_highest_balance(void *arg) {
	double max_balance = balance_array[0];

	for (int i = 1; i < ARRAY_SIZE; i++) {
		if (balance_array[i] > max_balance) {
			max_balance = balance_array[i];
		}
	}

	highest_balance = max_balance;
	pthread_exit(NULL);
}

void* search_average_balance(void *arg) {
	double average = 0;

	for (int i = 0; i < ARRAY_SIZE; i++) {
		average += balance_array[i];
	}

	average_balance = average / ARRAY_SIZE;
	pthread_exit(NULL);
}

int main() {
	pthread_t threads[NUMBER_THREADS];
	int i;
	srand(time(NULL));

    for (int i = 0; i < ARRAY_SIZE; i++) {
        balance_array[i] = (double) (rand() % 10000);
    }

	if (pthread_create(&threads[0], NULL, search_lowest_balance, NULL) != 0) {
		perror("Error creating thread!");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&threads[1], NULL, search_highest_balance, NULL) != 0) {
		perror("Error creating thread!");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&threads[2], NULL, search_average_balance, NULL) != 0) {
		perror("Error creating thread!");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < NUMBER_THREADS; i++) {
		if (pthread_join(threads[i], NULL) != 0) {
			perror("Error joining thread!");
 			exit(EXIT_FAILURE);
		}
	}

	printf("Lowest balance: %.2f\n", lowest_balance);
	printf("Highest balance: %.2f\n", highest_balance);
	printf("Average balance: %.2f\n", average_balance);

	return 0;
}