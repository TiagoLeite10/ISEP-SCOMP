#include <stdio.h>
#include <stdlib.h>
#include "generate_random_values.h"
#include "search_value.h"

#define ARRAY_SIZE 1000 
#define LIMIT_NUMBER 10000 

int main(void) {
    int numbers[ARRAY_SIZE];
    generate_random_values(numbers, ARRAY_SIZE, LIMIT_NUMBER);

    // Inicializa n
    int n = rand () % LIMIT_NUMBER;

    printf("O número %d encontra-se no array %d vezes!\n", n, search_value(numbers, ARRAY_SIZE, n));

    return 0;
}