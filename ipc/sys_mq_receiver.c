#include <stdio.h>
#include <stdlib.h>
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

    printf("接收端启动（按 Ctrl+C 停止）...\n");

    while (1) {
        // 持续接收类型 1 的消息
        if (msgrcv(msgid, &message, sizeof(message.msg_text), 1, 0) == -1) {
            perror("msgrcv");
            break;
        }
        printf("收到: %s\n", message.msg_text);
    }

    return 0;
}