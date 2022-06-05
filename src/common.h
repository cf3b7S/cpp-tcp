#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

// #define _GNU_SOURCE
// #include <pthread.h>
// #include <sched.h>  //cpu_set_t , CPU_SET
// #include <stdio.h>

#define PORT 8000
#define BACKLOG 5
#define EPOLL_MAX_EVENTS 10
// #define BUFSIZE 8388608
#define BUFSIZE 5000000

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

int getSoBuffSize(int socketfd, int optname) {
    int bufsize = 0;
    socklen_t bufLen = sizeof(bufsize);
    getsockopt(socketfd, SOL_SOCKET, optname, &bufsize, &bufLen);
    return bufsize;
}
// optname: SO_SNDBUF, SO_RCVBUF
void setSoBuffSize(int socketfd, int optname, int bufsize) {
    // socklen_t optlen = sizeof(bufsize);
    if (setsockopt(socketfd, SOL_SOCKET, optname, &bufsize, sizeof(int)) == -1) {
        std::cerr << "set buff size fail: " << strerror(errno) << std::endl;
    }
}

// int stick_this_thread_to_core(int core_id) {
//     int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
//     if (core_id < 0 || core_id >= num_cores) return EINVAL;

//     cpu_set_t cpuset;
//     CPU_ZERO(&cpuset);
//     CPU_SET(core_id, &cpuset);

//     pthread_t current_thread = pthread_self();
//     return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
// }

// int recvBuf = 0;
// socklen_t recvBufLen = sizeof(recvBufLen);
// getsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &recvBuf, &recvBufLen);
// recvBuf = 212992;
// recvBufLen = sizeof(recvBufLen);
// if (setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &recvBuf, recvBufLen) == -1) {
//     std::cerr << "set recv buff fail: " << strerror(errno) << std::endl;
// }
