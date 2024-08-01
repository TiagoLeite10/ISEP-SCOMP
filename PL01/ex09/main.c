#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUMBER_CHILDREN 10

void func_print_number(int n) {
    int limit = n * 100;

    for (int i = limit - 99; i <= limit; i++) {
        printf("%d\n", i);
    }
}

int main(void) {
    pid_t p[NUMBER_CHILDREN];

    for (int i = 0; i < NUMBER_CHILDREN; i++) {
        p[i] = fork();

        /* Caso aconteça uma falha na criação do processo filho */
        if (p[i] == -1) {
            perror("Fork falhou");
            exit(EXIT_FAILURE);
        }

        if (p[i] == 0){
            func_print_number(i + 1);

            exit(0);
        }
    }

    for (int i = 0; i < NUMBER_CHILDREN; i++) {
        waitpid(p[i], NULL, 0);
    }

    printf("Eu sou o pai e aguardei pela conclusão de todos os processos filhos!\n");
}