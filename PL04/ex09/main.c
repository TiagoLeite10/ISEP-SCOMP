#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include "../libs/my_semaphore_lib.h"

#define NUM_CHILDS_INDEX 1
#define SLEEP_TIME_INDEX 2
#define UNIT_OF_TIME_INDEX 3
#define TOTAL_NUM_OF_ARGS 4
#define SEM_NAME "/ex09_sem"
#define ERROR_VALUE -1

int main(int argc, char *argv[]) {

    if (argc != TOTAL_NUM_OF_ARGS) {
        perror("Número de argumentos errado!\n");
        exit(EXIT_FAILURE);
    }

    // Tratar o argumento do número de filhos
    char num_of_childs_string[10];
    strcpy(num_of_childs_string, argv[NUM_CHILDS_INDEX]);
    int num_of_childs = strtol(num_of_childs_string, NULL, 10);
    if ((errno == ERANGE && (num_of_childs == LONG_MAX || num_of_childs == LONG_MIN)) || (errno != 0 && num_of_childs == 0)) {
        perror("An error occurred while doing the strtol!\n");
        exit(EXIT_FAILURE);
    }

    // Tratar o argumento do tempo total de execução
    char sleep_time_string[10];
    strcpy(sleep_time_string, argv[SLEEP_TIME_INDEX]);
    int sleep_time = strtol(sleep_time_string, NULL, 10);
    if ((errno == ERANGE && (sleep_time == LONG_MAX || sleep_time == LONG_MIN)) || (errno != 0 && sleep_time == 0)) {
        perror("An error occurred while doing the strtol!\n");
        exit(EXIT_FAILURE);
    }

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 0;

    if (strcmp(argv[UNIT_OF_TIME_INDEX], "s") == 0) {
        ts.tv_sec = sleep_time;    
    } else if (strcmp(argv[UNIT_OF_TIME_INDEX], "ms") == 0) {
        sleep_time *= 1000000;
        ts.tv_nsec = sleep_time;
    } else if (strcmp(argv[UNIT_OF_TIME_INDEX], "us") == 0) {
        sleep_time *= 1000;
        ts.tv_nsec = sleep_time;
    } else {
        perror("As unidades de tempo suportadas são: us (microsegundos), ms (milisegundos) e s (segundos).\n");
        exit(EXIT_FAILURE);
    }

    sem_t *sem = semaphore_open_with_validation(SEM_NAME, O_CREAT, 0644, 0);
    
    // Tempo de execução do programa: https://www.techiedelight.com/pt/find-execution-time-c-program/
    struct timeval start, end;
    gettimeofday(&start, NULL);

    pid_t p;
    for (int i = 0; i < num_of_childs; i++) {
        p = fork();
        /* Caso aconteça uma falha na criação do processo filho */
        if (p == ERROR_VALUE) {
            perror("Fork failed!\n");
            exit(EXIT_FAILURE);
        }

        if (p == 0)
            break;
    }
    
    if (p == 0) {
        nanosleep(&ts, NULL);
        sem_post_with_validation(sem);
    } else {
        for (int i = 0; i < num_of_childs; i++)
            sem_wait_with_validation(sem);
        
        gettimeofday(&end, NULL);
        long seconds = (end.tv_sec - start.tv_sec);
        long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
        long milliseconds = micros * 0.001;
    
        printf("O tempo de execução do programa foi de %ld segundos e %ld microsegundos (%ld milisegundos)\n", seconds, micros, milliseconds);

        semaphore_unlink_with_validation(SEM_NAME);
    }

    semaphore_close_with_validation(sem);

    return 0;

}