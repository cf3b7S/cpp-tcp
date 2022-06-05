#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "common.h"
using namespace std;
using namespace chrono;

int connect(int socketfd, const char* ip, int port) {
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(ip);
    return ::connect(socketfd, (struct sockaddr*)&sa, sizeof(sa));
}

int main() {
    std::cout << "This is client" << std::endl;

    // create socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        std::cerr << "create socket fail: " << strerror(errno) << std::endl;
        return -1;
    }

    // connect
    if (connect(clientfd, "127.0.0.1", PORT) < 0) {
        std::cerr << "connect fail:" << strerror(errno) << std::endl;
        return -1;
    }

    // set socket flag noblock
    if (!setSocketFlag(clientfd, O_NONBLOCK)) {
        return -1;
    }

    char data[1024 * 1024];
    // char buf[1024 * 1024];
    char d[100];
    memset(data, 0, sizeof(data));
    // std::cout << sizeof(data) << std::endl;

    int cnt = 100000;
    auto start = system_clock::now();
    while (cnt > 0) {
        // std::cin >> d;
        int total = sizeof(data);
        int remain = total;
        char* p = data;
        while (true) {
            int len = send(clientfd, p, remain, 0);
            // if (len == -1) {
            //     std::cerr << "send fail:" << strerror(errno) << std::endl;
            // } else {
            //     std::cout << len << " bytes sended" << std::endl;
            // }
            remain = remain - len;
            if (remain == 0) {
                break;
            } else {
                p += len;
            }
        }
        cnt -= 1;
    }
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    std::cout << double(duration.count()) / 100000.0 << "μs" << std::endl;
    close(clientfd);
    return 0;
}