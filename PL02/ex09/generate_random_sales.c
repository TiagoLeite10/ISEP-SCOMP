#include <stdlib.h>
#include <time.h>
#include "sale.h"

/* Função para inicializar um array de vendas com valores aleatórios */
void generate_random_sales(sale *sales, int size) {
	time_t t; /* needed to init. the random number generator (RNG) */
	
	/* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand ((unsigned) time (&t));	
	
	/* Preencher os campos da estrutura com valores aleatórios */
	for (int i = 0; i < size; i++) {
		sales[i].customer_code = rand() % 1000;
		sales[i].product_code = i;
		sales[i].quantity = rand() % 22;
	}
} 