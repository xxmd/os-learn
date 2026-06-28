#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    printf("=== 进程A（创建者） ===\n");
    printf("管道创建成功！读端 fd = %d, 写端 fd = %d\n", fd[0], fd[1]);
    printf("我的进程号 PID = %d\n\n", getpid());

    // 故意不关闭任何一端，保持管道打开
    printf("进程A正在等待...（按 Ctrl+C 结束）\n");
    while (1) {
        sleep(10);
    }

    return 0;
}