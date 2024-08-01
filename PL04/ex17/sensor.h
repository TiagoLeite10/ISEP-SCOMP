#ifndef SENSOR_H
#define SENSOR_H

    typedef struct {
        int sensor_id;
        int actual_read;
        int last_read;
        int is_in_alarm_mode;
    } sensor;
    
#endif
