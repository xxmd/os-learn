#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      // sleep
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "msg.h"

int main() {
    int msgid;
    struct msg_buffer message;

    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("发送端启动（每 3 秒发送一次当前时间），按 Ctrl+C 停止...\n");

    message.msg_type = 1;   // 使用固定类型 1

    while (1) {
        // 获取当前时间并格式化
        time_t rawtime;
        struct tm *timeinfo;
        char time_str[64];

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

        snprintf(message.msg_text, MAX_TEXT, "当前时间: %s", time_str);

        if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
            perror("msgsnd");
            break;
        }

        printf("已发送: %s\n", message.msg_text);
        sleep(3);   // 每隔 3 秒
    }

    return 0;
}