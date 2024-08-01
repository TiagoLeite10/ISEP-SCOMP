#include <stdio.h>
#include "generate_random_values.h"
#include "perform_calculation.h"
#include "search_max_value.h"

#define ARRAY_SIZE 1000 
#define LIMIT_NUMBER 256
#define NUMBER_CHILDREN 5

int main(void) {
    int numbers[ARRAY_SIZE];
    generate_random_values(numbers, ARRAY_SIZE, LIMIT_NUMBER);

    int max_value = search_max_value(numbers, ARRAY_SIZE, NUMBER_CHILDREN);
    printf("Valor máximo encontrado no array: %d\n", max_value);

    perform_calculation(numbers, ARRAY_SIZE, max_value);

    return 0;
}