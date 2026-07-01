// mutex_infinite.c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define NUM_THREADS 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
volatile long long counter = 0;
volatile int running = 1;

void sig_handler(int sig) {
    running = 0;
}

void* thread_func(void* arg) {
    while (running) {
        pthread_mutex_lock(&mutex);
        counter++;
        usleep(1000);   // 100微秒，改成1~500看看效果
        pthread_mutex_unlock(&mutex);

        // 可微调竞争强度（改小 = 竞争更激烈）
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    signal(SIGINT, sig_handler);

    printf("=== 互斥锁持续运行中 (按 Ctrl+C 停止) ===\n");
    printf("线程数: %d\n", NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }

    // 主线程每秒打印一次计数器
    while (running) {
        long long prev = counter;
        sleep(1);
        printf("Counter/s: %lld\n", counter - prev);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    return 0;
}