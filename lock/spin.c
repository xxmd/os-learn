// spin_infinite.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define NUM_THREADS 8

pthread_spinlock_t spinlock;
volatile long long counter = 0;
volatile int running = 1;

void sig_handler(int sig) {
    running = 0;
}

void* thread_func(void* arg) {
    while (running) {
        pthread_spin_lock(&spinlock);
        counter++;
        pthread_spin_unlock(&spinlock);

        usleep(100);     // 调整此值观察差异
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    signal(SIGINT, sig_handler);

    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

    printf("=== 自旋锁持续运行中 (按 Ctrl+C 停止) ===\n");
    printf("线程数: %d\n", NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }

    while (running) {
        long long prev = counter;
        sleep(1);
        printf("Counter/s: %lld\n", counter - prev);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_spin_destroy(&spinlock);
    return 0;
}