#include <cstdlib>
#include <stdio.h>


typedef struct {
    int programCounter;
    int argument;
    int result;
} Frame;

int computeCountSum(int count) {
    Frame stack[32];
    Frame *top = stack;
    *top = (Frame){.programCounter = 0, .argument = count};
    for (Frame *frame; (frame = top) >= stack; frame->programCounter++) {
        switch (frame->programCounter) {
            case 0:
                if (frame->argument == 1) {
                    frame->result = 1;
                    frame->programCounter = 1;
                }
                break;
            case 1:
                // Frame *temp = frame;
                // 压栈
                top++;
                *top = (Frame){.programCounter = 0, .argument = frame->argument - 1};
                // temp->result = temp->argument + top->result;
                break;
            case 2:
                if (top > stack) {
                    Frame *pre = top - 1;
                    pre->result = pre->argument + top->result;
                }
                top--;
                break;
        }
    }
    top++;
    return top->result;
}


int main() {
    int result = computeCountSum(5);
    printf("%d\n", result);
    return 0;
}
