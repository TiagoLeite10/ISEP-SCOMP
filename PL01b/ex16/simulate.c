#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_TIME_TO_SLEEP 2
#define LOWER_INTERVAL_OF_SUCCESS 5
#define HIGHER_INTERVAL_OF_SUCCESS 10
#define MAX_INTERVAL_SIMULATE1 100
#define MAX_INTERVAL_SIMULATE2 10

int simulate(int value) {
    // Em caso de sucesso
    if (value >= LOWER_INTERVAL_OF_SUCCESS && value <= HIGHER_INTERVAL_OF_SUCCESS) {
        return 1;
    }

    return 0;
}

int simulate1() {
    srand(time(NULL));
    int r = rand() % MAX_INTERVAL_SIMULATE1 + 1;
    return simulate(r);    
}

int simulate2() {
    srand(time(NULL));
    int r = rand() % MAX_INTERVAL_SIMULATE2 + 1;
    return simulate(r);
}
