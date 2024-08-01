#include <stdlib.h>
#include <time.h>

/* Função para inicializar um array com valores aleatórios */
void generate_random_values(int *array, int size) {
	time_t t; /* needed to init. the random number generator (RNG) */
	
	/* intializes RNG (srand():stdlib.h; time(): time.h) */
    	srand ((unsigned) time (&t));	
	
	/* Preencher os campos da estrutura com valores aleatórios */
	for (int i = 0; i < size; i++) {
		*array = rand() % 10;
		array++;
	}
} 
