#ifndef MSG_H
#define MSG_H

#define MSG_KEY 1234
#define MAX_TEXT 512

struct msg_buffer {
    long msg_type;
    char msg_text[MAX_TEXT];
};

#endif