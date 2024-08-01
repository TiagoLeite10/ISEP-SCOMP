#ifndef SHARED_DATA_TYPE_H
#define SHARED_DATA_TYPE_H
#define VETOR_SIZE 10
    typedef struct {
        int numbers[VETOR_SIZE];
        int num_readers_a;
        int num_readers_b;
    } shared_data_type;
#endif