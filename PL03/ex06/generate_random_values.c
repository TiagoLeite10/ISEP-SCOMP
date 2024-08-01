#include <stdlib.h>
#include <time.h>

/* Função para inicializar um array com números aleatórios entre 0 e um determinado limite passado por parâmetro */
void generate_random_values(int *numbers, int size, int limit) {
    time_t t; /* needed to init. the random number generator (RNG) */
    int i;

    limit++; // Para que o limite seja respeitado como inclusivo.

    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand ((unsigned) time (&t));

    /* initialize array with random numbers (rand(): stdlib.h) */
    for (i = 0; i < size; i++) {
        numbers[i] = rand () % limit;
    }
}

