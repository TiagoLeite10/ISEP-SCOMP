#ifndef SHARED_DATA_TYPE_H
#define SHARED_DATA_TYPE_H
#define INITIAL_VETOR_SIZE 10000
#define FINAL_VETOR_SIZE 1000
    typedef struct {
        int initial_vector[INITIAL_VETOR_SIZE];
        int final_vector[FINAL_VETOR_SIZE];
        int max_number;
        int next_position;
        int flag;
    } shared_data_type;
#endif