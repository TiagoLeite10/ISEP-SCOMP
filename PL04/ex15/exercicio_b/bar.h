#ifndef BAR_H
#define BAR_H
    #include <semaphore.h>
    #define NUM_MAX_CLIENTS_IN_QUEUE 50
    #define NORMAL_CLIENT_INDEX 0
    #define SPECIAL_CLIENT_INDEX 1
    #define VIP_CLIENT_INDEX 2
    #define NUM_OF_CLIENTS_TYPE 3
    typedef struct {
        sem_t *clients_queue[NUM_OF_CLIENTS_TYPE][NUM_MAX_CLIENTS_IN_QUEUE];
        int next_queue_client_index[NUM_OF_CLIENTS_TYPE];
        int next_client_to_enter_in_bar[NUM_OF_CLIENTS_TYPE];
    } bar;
    
#endif