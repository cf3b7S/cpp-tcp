#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
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
    // stick_this_thread_to_core(1);
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

    setSoBuffSize(clientfd, SO_SNDBUF, BUFSIZE);

    int sendBufSize = getSoBuffSize(clientfd, SO_SNDBUF);
    std::cout << "sendBufSize: " << sendBufSize << std::endl;

    int recvBufSize = getSoBuffSize(clientfd, SO_RCVBUF);
    std::cout << "recvBufSize: " << recvBufSize << std::endl;

    // int one = 1;
    // setsockopt(clientfd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
    // setsockopt(clientfd, IPPROTO_TCP, TCP_QUICKACK, &one, sizeof(one));

    memset(msg, 0, sizeof(msg));
    // std::cout << sizeof(data) << std::endl;

    int cnt = 100000;
    auto start = system_clock::now();
    while (cnt > 0) {
        // std::cin >> d;
        int total = sizeof(msg);
        int remain = total;
        char* p = msg;
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

// TCP_CORK 阻止立即发送
// int state = 1;
// setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
// write(http_resp_header);
// sendfile(sockfd, fd, &off, len);
// state = 0;
// setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &state, sizeof(state));
