#ifndef COMMAND_H
#define COMMAND_H
    typedef struct {
        char cmd[32]; // Command name.
        int time_cap ; // Time limit to complete (in seconds).
    } command_t;
#endif