#ifndef MY_SHARED_MEMORY_LIB_H
#define MY_SHARED_MEMORY_LIB_H
    int shm_open_with_validations(const char *shm_name, int oflag, mode_t mode);
    void ftruncate_with_validations(int fd, off_t data_size);
    void *mmap_with_validations(void *addr, size_t data_size, int prot, int flags, int fd, off_t offset);
    void shm_unlink_with_validations(char *shm_name);
    void munmap_with_validations(void *shared_data, size_t data_size);
    void close_with_validations(int fd);
#endif