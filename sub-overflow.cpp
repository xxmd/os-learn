#include <stdio.h>
#include <pthread.h>

#define N 100000000

long sum = 0;

void* Tsum(void* arg) {
    for (int i = 0; i < N; i++) {
        asm volatile("add $1, %0" : "+m"(sum));
        // sum++;   // 原来的普通自增（已被注释）
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // 创建两个线程
    pthread_create(&t1, NULL, Tsum, NULL);
    pthread_create(&t2, NULL, Tsum, NULL);

    // 等待两个线程执行完成
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("sum = %ld\n", sum);

    return 0;
}