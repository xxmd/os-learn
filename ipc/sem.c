#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>

#define BUFFER_SIZE 5
#define SEM_MUTEX "/sem_mutex"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL  "/sem_full"
#define SHM_NAME  "/pc_shm"

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
    int produced;
    int consumed;
} SharedData;

// 打印当前缓冲区内容
void print_buffer(SharedData *shdata) {
    printf("缓冲区 [ ");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (i == shdata->in && i == shdata->out) {
            printf("*%d* ", shdata->buffer[i]);  // in 和 out 重合
        } else if (i == shdata->in) {
            printf(">%d< ", shdata->buffer[i]);  // 生产者位置
        } else if (i == shdata->out) {
            printf("[%d] ", shdata->buffer[i]);  // 消费者位置
        } else {
            printf("%d ", shdata->buffer[i]);
        }
    }
    printf("]  (in=%d, out=%d)\n", shdata->in, shdata->out);
}

int main() {
    sem_t *mutex = sem_open(SEM_MUTEX, O_CREAT | O_RDWR, 0666, 1);
    sem_t *empty = sem_open(SEM_EMPTY, O_CREAT | O_RDWR, 0666, BUFFER_SIZE);
    sem_t *full  = sem_open(SEM_FULL,  O_CREAT | O_RDWR, 0666, 0);

    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    SharedData *shdata = (SharedData*)mmap(NULL, sizeof(SharedData),
                                           PROT_READ | PROT_WRITE,
                                           MAP_SHARED, shm_fd, 0);
    if (shdata == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 初始化
    for (int i = 0; i < BUFFER_SIZE; i++) shdata->buffer[i] = 0;
    shdata->in = 0;
    shdata->out = 0;
    shdata->produced = 0;
    shdata->consumed = 0;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    srand(time(NULL));

    if (pid == 0) {
        // ==================== 消费者 ====================
        printf("【消费者】启动...\n");
        for (int i = 0; i < 10; i++) {
            sem_wait(full);
            sem_wait(mutex);

            int item = shdata->buffer[shdata->out];
            shdata->out = (shdata->out + 1) % BUFFER_SIZE;
            shdata->consumed++;

            printf("【消费者】取出: %2d   ", item);
            print_buffer(shdata);

            sem_post(mutex);
            sem_post(empty);

            usleep(rand() % 600000);
        }
        exit(0);
    }
    else {
        // ==================== 生产者 ====================
        printf("【生产者】启动...\n");
        for (int i = 0; i < 10; i++) {
            int item = rand() % 100 + 1;

            sem_wait(empty);
            sem_wait(mutex);

            shdata->buffer[shdata->in] = item;
            shdata->in = (shdata->in + 1) % BUFFER_SIZE;
            shdata->produced++;

            printf("【生产者】放入: %2d   ", item);
            print_buffer(shdata);

            sem_post(mutex);
            sem_post(full);

            usleep(rand() % 500000);
        }

        wait(NULL);
        printf("\n=== 生产消费完成！共生产 %d 个，消费 %d 个 ===\n",
               shdata->produced, shdata->consumed);
    }

    // 清理
    sem_close(mutex);
    sem_close(empty);
    sem_close(full);
    munmap(shdata, sizeof(SharedData));

    if (pid > 0) {
        sem_unlink(SEM_MUTEX);
        sem_unlink(SEM_EMPTY);
        sem_unlink(SEM_FULL);
        shm_unlink(SHM_NAME);
    }

    return 0;
}