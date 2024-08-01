#ifndef METRO_DATA_H
#define METRO_DATA_H
    #define NUM_PASSENGERS 20
    #define STATION_A 1
    #define STATION_B 2
    #define STATION_OTHER 3
    #define DOOR_1 0
    #define DOOR_2 1
    #define DOOR_3 2
    #define NUMBER_OF_DOORS 3
    #define MAXIMUM_CAPACITY_OF_THE_METRO 20
    typedef struct {
        sem_t *passangers_position[NUM_PASSENGERS];
        int passengers_inside_metro;
        int num_new_passangers;
        int actual_station;
        pid_t pid_passengers[MAXIMUM_CAPACITY_OF_THE_METRO];
        int write_index;
    } metro_data;
    
#endif