#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <errno.h>

int main(void) {
    struct pollfd pfd;

    pfd.fd = STDIN_FILENO;          // 監看stdin
    pfd.events = POLLIN;


    int timeout_ms = 5000; // 最多等 5 秒
    int n = poll(&pfd, 1, timeout_ms);
    if (n < 0) {
        perror("poll");
        return 1;
    } else if (n == 0) {
        printf("在這5秒stdin都沒有任何事件\n");
    } else {
        if (pfd.revents & POLLIN) {
            printf("stdin 可讀\n");
        }
    }
    return 0;
}