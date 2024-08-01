#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

void task_a() {
	sleep(3);

	printf("Task A: done!\n");
}

void task_b() {
    srand(time(NULL));
	sleep(rand() % 5 + 1);
	
	printf("Task B: done!\n");
}

void task_c() {
	sleep(1);

	printf("Task C: done!\n");
}