#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int pipe_test() {
    int fd[2];
    int ret = pipe(fd);
    if (ret == -1) {
        printf("pipe failed\n");
        return -1;
    }
    int pid = fork();
    if (pid == 0) {
        printf("child\n");
        close(fd[0]);
        while (1) {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char buf[32];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", t);
            write(fd[1], buf, strlen(buf));
            sleep(3);
        }
    } else if (pid > 0) {
        printf("parent\n");
        close(fd[1]);
        while (1) {
            char buf[1024];
            read(fd[0], buf, sizeof(buf));
            printf("receive %s\n", buf);
        }
    } else {
        printf("fork failure\n");
    }
}

int main(int argc, char *argv[]) {
    pipe_test();
    return 0;
}
