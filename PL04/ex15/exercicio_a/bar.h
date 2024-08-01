#ifndef BAR_H
#define BAR_H
    #include <semaphore.h>
    #define NUM_MAX_CLIENTS_IN_QUEUE 50
    typedef struct {
        sem_t *clients_queue[NUM_MAX_CLIENTS_IN_QUEUE];
        int next_queue_client_index;
        int next_client_to_enter_in_bar;
    } bar;
    
#endif