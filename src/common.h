#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define PORT 8000
#define BACKLOG 5
#define EPOLL_MAX_EVENTS 10
// #define BUFSIZE 131070 // 331.489μs
// #define BUFSIZE 109225  // 331.258μs
// #define BUFSIZE 90225  // 400.894μs
#define BUFSIZE 87380  // 179μs
// #define BUFSIZE 43690  // 683.645μs

char msg[1024 * 1024];
// char msg[128];

int getSocketFlag(int socketfd) {
    int flags = fcntl(socketfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "get socket flags fail: " << strerror(errno) << std::endl;
    }
    return flags;
}

bool setSocketFlag(int socketfd, int flag) {
    int flags = getSocketFlag(socketfd);
    if (flags == -1) {
        return false;
    }
    flags |= flag;
    int s = fcntl(socketfd, F_SETFL, flags);
    if (s == -1) {
        std::cerr << "[E] fcntl failed (F_SETFL)" << std::endl;
        return false;
    }
    return true;
}

// optname: SO_SNDBUF, SO_RCVBUF
void setSocketBuff(int socketfd, int optname, int bufsize) {
    // socklen_t optlen = sizeof(bufsize);
    if (setsockopt(socketfd, SOL_SOCKET, optname, &bufsize, sizeof(int)) == -1) {
        std::cerr << "set buff size fail: " << strerror(errno) << std::endl;
    }
}

// int recvBuf = 0;
// socklen_t recvBufLen = sizeof(recvBufLen);
// getsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &recvBuf, &recvBufLen);
// recvBuf = 212992;
// recvBufLen = sizeof(recvBufLen);
// if (setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &recvBuf, recvBufLen) == -1) {
//     std::cerr << "set recv buff fail: " << strerror(errno) << std::endl;
// }
