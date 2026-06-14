#include <stdio.h>
#include <assert.h>

// 栈帧结构体：模拟每次函数调用的现场
typedef struct {
    int pc, n;
    char from, to, via;
} Frame;

// 宏：模拟函数调用call，压栈，初始化pc=0，可变参数给结构体成员赋值
#define call(...) ({ *(++top) = (Frame){.pc = 0, __VA_ARGS__}; })
// 宏：模拟return出栈
#define ret() ({ top--; })
// 宏：修改当前栈帧pc，实现跳转loc行（case编号）
#define goto(loc) ({ f->pc = (loc) - 1; })

// 非递归版汉诺塔（手动模拟调用栈，不用系统递归）
void hanoi(int n, char from, char to, char via) {
    Frame stk[64], *top = stk - 1; // 栈空间，top初始在栈外
    call(n, from, to, via);        // 压入初始调用帧

    // 主循环：遍历栈里所有帧，pc自增模拟程序计数器
    for (Frame *f; (f = top) >= stk; f->pc++) {
        switch (f->pc) {
            case 0:
                // n==1直接打印，跳转到case4返回
                if (f->n == 1) {
                    printf("%c -> %c\n", f->from, f->to);
                    goto(4);
                }
                break;
            case 1:
                // hanoi(n-1, from, via, to) 递归1
                call(f->n - 1, f->from, f->via, f->to);
                break;
            case 2:
                // hanoi(1, from, to, via) 递归2
                call(1, f->from, f->to, f->via);
                break;
            case 3:
                // hanoi(n-1, via, to, from) 递归3
                call(f->n - 1, f->via, f->to, f->from);
                break;
            case 4:
                // 函数返回，出栈
                ret();
                break;
            default:
                assert(0);
        }
    }
}

int main(void) {
    // 测试：3个盘子，A→C，辅助柱B
    hanoi(3, 'A', 'C', 'B');
    return 0;
}