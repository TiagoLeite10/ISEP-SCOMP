#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define ERROR_VALUE -1

sem_t *semaphore_open_with_validation(char* sem_name, int oflag, mode_t mode, unsigned int initial_value) {
	sem_t *sem = sem_open(sem_name, oflag, mode, initial_value);
	
	if (sem == SEM_FAILED) {
		perror("An error occurred while trying to create the semaphore!\n");
		exit(EXIT_FAILURE);
	}

	return sem;
}

void semaphore_close_with_validation(sem_t *sem) {
    if (sem_close(sem) == ERROR_VALUE) {
        perror("An error occurred while trying to close semaphore!\n");
        exit(EXIT_FAILURE);
    }
}

void semaphore_unlink_with_validation(char* sem_name) {
    if (sem_unlink(sem_name) == ERROR_VALUE) {
        perror("An error occurred while trying to unlink the semaphore!\n");
		exit(EXIT_FAILURE);
    }
}

void sem_post_with_validation(sem_t *sem) {
	if (sem_post(sem) == ERROR_VALUE) {
		perror("An error occurred while trying to increment the semaphore!\n");
		exit(EXIT_FAILURE);
	}
}

void sem_wait_with_validation(sem_t *sem) {
	if (sem_wait(sem) == ERROR_VALUE) {
		perror("An error occurred while trying to decrement the semaphore!\n");
		exit(EXIT_FAILURE);
	}
}

void sem_getvalue_with_validations(sem_t *sem, int *sval) {
	if (sem_getvalue(sem, sval) == ERROR_VALUE) {
		perror("An error occurred while trying to get the semaphore value!\n");
		exit(EXIT_FAILURE);
	}
}