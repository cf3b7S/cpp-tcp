#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define PORT 8000
#define BACKLOG 5
#define EPOLL_MAX_EVENTS 10

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
