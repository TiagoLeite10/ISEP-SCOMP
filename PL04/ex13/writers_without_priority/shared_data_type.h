#ifndef SHARED_DATA_TYPE_H
#define SHARED_DATA_TYPE_H
#define STRING_SIZE 50
    typedef struct {
        char phrase[STRING_SIZE];
        int num_writers;
        int num_readers;
    } shared_data_type;
#endif