#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int main() {
    const char *fifo = "/tmp/myfifo";
    
    // 创建 FIFO（如果不存在）
    mkfifo(fifo, 0666);
    
    printf("FIFO 创建成功: %s\n", fifo);
    printf("等待读取进程启动...\n");
    
    int fd = open(fifo, O_WRONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    char *messages[] = {
        "Hello, FIFO!",
        "这是第二条消息",
        "FIFO 测试 - 第三条",
        "程序即将结束"
    };
    
    for (int i = 0; i < 4; i++) {
        write(fd, messages[i], strlen(messages[i]) + 1);
        printf("已写入: %s\n", messages[i]);
        sleep(1);  // 每隔1秒写一条
    }
    
    close(fd);
    printf("写入完成\n");
    return 0;
}