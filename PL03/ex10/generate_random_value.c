#include <stdlib.h>
#include <time.h>

/* Função para gerar um número aleatório entre dois limites passados por parâmetro */
int generate_random_value(int initial_limit, int final_limit) {
    static int initialized = 0;

    if (!initialized) {
        /* needed to init. the random number generator (RNG) */
        time_t t;
        
        /* intializes RNG (srand():stdlib.h; time(): time.h) */
        srand ((unsigned) time (&t));
        
        initialized = 1;
    }

    /* initialize number with random numbers (rand(): stdlib.h) */
    return (rand() % final_limit) + initial_limit;
}
