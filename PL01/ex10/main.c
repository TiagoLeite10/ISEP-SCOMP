#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

#include "generate_random_values.h"
#include "find_first_occurrence.h"

#define ARRAY_SIZE 2000
#define LIMIT_NUMBER_EXCLUSIVE 101
#define NUM_CHILD_PROCESSES 10
#define NUMBER_TO_FIND 100
#define NOT_FOUND_VALUE 255

int main(void) {
    	int numbers[ARRAY_SIZE];
    	generate_random_values(numbers, ARRAY_SIZE, LIMIT_NUMBER_EXCLUSIVE);
	int *numbers_ptr;
	numbers_ptr = numbers;

	pid_t p[NUM_CHILD_PROCESSES];

	int interval_size = ARRAY_SIZE / NUM_CHILD_PROCESSES;

	for (int i = 0; i < NUM_CHILD_PROCESSES; i++) {
		p[i] = fork();

		if (p[i] == 0) {
			int exit_value = find_first_occurrence(numbers_ptr, interval_size, NUMBER_TO_FIND);
			
			if (exit_value == -1) {
				exit_value = NOT_FOUND_VALUE;
			}

			exit(exit_value);
		}

		numbers_ptr += interval_size;
	}
	
	int status;
	for (int i = 0; i < NUM_CHILD_PROCESSES; i++) {
		waitpid(p[i], &status, 0);
		printf("O processo nº %d terminou com o valor %d.\n", (i+1), WEXITSTATUS(status));
	}

	printf("Sou o processo pai e vou terminar agora!\n");

    	return 0;
}
