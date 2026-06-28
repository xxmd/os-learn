#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    const char *fifo = "/tmp/myfifo";
    char buffer[100];
    
    printf("等待写入进程...\n");
    
    int fd = open(fifo, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    printf("开始读取 FIFO...\n");
    
    while (1) {
        ssize_t bytes = read(fd, buffer, sizeof(buffer));
        if (bytes > 0) {
            printf("读取到: %s\n", buffer);
        } else if (bytes == 0) {
            // 写入端已关闭
            break;
        } else {
            perror("read");
            break;
        }
    }
    
    close(fd);
    printf("读取结束\n");
    return 0;
}