#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>

#define BUFFER_SIZE 5          // 缓冲区大小
#define SEM_MUTEX "/sem_mutex" // 互斥信号量
#define SEM_EMPTY "/sem_empty" // 空槽位信号量
#define SEM_FULL  "/sem_full"  // 满槽位信号量
#define SHM_NAME  "/pc_shm"    // 共享内存名称

// 共享数据结构
typedef struct {
    int buffer[BUFFER_SIZE];   // 环形缓冲区
    int in;                    // 生产者写入位置
    int out;                   // 消费者读取位置
    int produced;              // 已生产计数
    int consumed;              // 已消费计数
} SharedData;

int main() {
    // 创建/打开信号量
    sem_t *mutex = sem_open(SEM_MUTEX, O_CREAT | O_RDWR, 0666, 1);   // 初值1（互斥）
    sem_t *empty = sem_open(SEM_EMPTY, O_CREAT | O_RDWR, 0666, BUFFER_SIZE); // 初值=缓冲区大小
    sem_t *full  = sem_open(SEM_FULL,  O_CREAT | O_RDWR, 0666, 0);   // 初值0（初始无数据）

    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // 创建共享内存
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(SharedData));
    SharedData *shdata = (SharedData*)mmap(NULL, sizeof(SharedData),
                                           PROT_READ | PROT_WRITE,
                                           MAP_SHARED, shm_fd, 0);
    if (shdata == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // 初始化共享数据
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
        // ==================== 子进程：消费者 ====================
        printf("消费者启动...\n");
        for (int i = 0; i < 10; i++) {   // 消费10次
            sem_wait(full);              // 等待有数据
            sem_wait(mutex);             // 进入临界区

            int item = shdata->buffer[shdata->out];
            shdata->out = (shdata->out + 1) % BUFFER_SIZE;
            shdata->consumed++;

            printf("消费者取出: %d (已消费 %d 个)\n", item, shdata->consumed);

            sem_post(mutex);             // 离开临界区
            sem_post(empty);             // 增加一个空槽位

            usleep(rand() % 500000);     // 模拟消费时间
        }
        exit(0);
    } 
    else {
        // ==================== 父进程：生产者 ====================
        printf("生产者启动...\n");
        for (int i = 0; i < 10; i++) {   // 生产10次
            int item = rand() % 100 + 1; // 产生随机数

            sem_wait(empty);             // 等待有空槽位
            sem_wait(mutex);             // 进入临界区

            shdata->buffer[shdata->in] = item;
            shdata->in = (shdata->in + 1) % BUFFER_SIZE;
            shdata->produced++;

            printf("生产者放入: %d (已生产 %d 个)\n", item, shdata->produced);

            sem_post(mutex);             // 离开临界区
            sem_post(full);              // 增加一个满槽位

            usleep(rand() % 400000);     // 模拟生产时间
        }

        wait(NULL);  // 等待消费者结束
        printf("生产消费完成！共生产 %d 个，消费 %d 个\n", 
               shdata->produced, shdata->consumed);
    }

    // 清理资源
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