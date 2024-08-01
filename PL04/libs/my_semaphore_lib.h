#ifndef MY_SEMAPHORE_LIB_H
#define MY_SEMAPHORE_LIB_H
#include <semaphore.h>
    sem_t *semaphore_open_with_validation(char* sem_name, int oflag, mode_t mode, unsigned int initial_value);
    void semaphore_close_with_validation(sem_t *sem);
    void semaphore_unlink_with_validation(char* sem_name);
    void sem_post_with_validation(sem_t *sem);
    void sem_wait_with_validation(sem_t *sem);
    void sem_getvalue_with_validations(sem_t *sem, int *sval);
#endif