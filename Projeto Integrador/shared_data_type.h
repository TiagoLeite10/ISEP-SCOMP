#ifndef SHARED_DATA_TYPE_H
#define SHARED_DATA_TYPE_H
#define BUFFER_SIZE 5
    typedef struct {
        int data[BUFFER_SIZE];
        int write_index;
    } shared_data_type;
#endif