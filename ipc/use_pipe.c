#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    // 假设我们“知道”另一个进程的 fd 号（实际中这是不可能直接知道的，这里硬编码演示）
    int read_fd = 3;   // 通常 pipe 第一个 fd 是 3
    int write_fd = 4;

    printf("=== 进程B（无关进程） ===\n");
    printf("尝试直接使用 fd %d 读取管道...\n", read_fd);
    printf("我的进程号 PID = %d\n\n", getpid());

    char buf[100];
    ssize_t n = read(read_fd, buf, sizeof(buf));

    if (n == -1) {
        perror("read 失败");
        printf("失败原因：文件描述符 %d 在本进程中不存在或无效\n", read_fd);
    } else {
        printf("成功读取：%s\n", buf);
    }

    return 0;
}