#ifndef SHARED_DATA_TYPE_H
#define SHARED_DATA_TYPE_H
#define ARRAY_SIZE 10
    typedef struct {
        int numbers[ARRAY_SIZE];
        int can_write;
        int number_elements;
    } shared_data_type;
#endif