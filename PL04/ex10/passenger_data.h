#ifndef PASSENGER_DATA_H
#define PASSENGER_DATA_H
    typedef struct {
        int leave_station;
        char leave_station_description[15];
        int choosen_door;
        int enter_station;
        char enter_station_description[15];
    } passenger_data;
    
#endif