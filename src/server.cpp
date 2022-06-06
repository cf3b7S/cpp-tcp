#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "common.h"

int acceptConn(int epollfd, int socketfd, struct epoll_event& event) {
    struct sockaddr_in sa;
    socklen_t saLen = sizeof(sa);
    int connfd = accept(socketfd, (struct sockaddr*)&sa, &saLen);
    if (connfd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {  // Done processing incoming connections
            return -1;
        } else {
            std::cerr << "accept fail: " << strerror(errno) << std::endl;
        }
    }

    std::string hbuf(NI_MAXHOST, '\0');
    std::string sbuf(NI_MAXSERV, '\0');
    if (getnameinfo((struct sockaddr*)&sa, saLen, const_cast<char*>(hbuf.data()), hbuf.size(),
                    const_cast<char*>(sbuf.data()), sbuf.size(),
                    NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
        std::cout << "[I] Accepted connection on fd " << socketfd << " from " << hbuf << ":" << sbuf
                  << std::endl;
    }
    return connfd;
}

int addEpollEvent(int epollfd, int socketfd) {
    struct epoll_event event;
    event.data.fd = socketfd;
    event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &event) == -1) {
        std::cerr << "add epoll event fail on fd: " << socketfd << ". " << strerror(errno)
                  << std::endl;
        return -1;
    }
    return 0;
}

int main() {
    // stick_this_thread_to_core(0);
    std::cout << "This is server" << std::endl;
    // create socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        std::cerr << "create socket fail: " << strerror(errno) << std::endl;
        return -1;
    }

    // bind
    struct sockaddr_in server_sa;
    server_sa.sin_family = AF_INET;
    server_sa.sin_port = htons(PORT);
    server_sa.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenfd, (struct sockaddr*)&server_sa, sizeof(server_sa)) == -1) {
        std::cerr << "bind socket fail: " << strerror(errno) << std::endl;
        return -1;
    }
    // set socket option SO_REUSEADDR SO_REUSEPORT
    if (!setSocketOpt(listenfd, SO_REUSEADDR | SO_REUSEPORT)) {
        return -1;
    }

    // int one = 1;
    // setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
    // setsockopt(listenfd, IPPROTO_TCP, TCP_QUICKACK, &one, sizeof(one));

    // listen
    if (listen(listenfd, BACKLOG) == -1) {
        std::cerr << "listen socket fail: " << strerror(errno) << std::endl;
        return 0;
    }

    // create epoll
    int epollfd = epoll_create(1);
    if (epollfd == -1) {
        std::cerr << "epoll create fail: " << strerror(errno) << std::endl;
        return -1;
    }

    // add epoll event
    if (addEpollEvent(epollfd, listenfd) == -1) {
        return -1;
    }

    int connfd;
    struct epoll_event events[EPOLL_MAX_EVENTS];
    int recved = 0;
    while (true) {
        auto n = epoll_wait(epollfd, events, EPOLL_MAX_EVENTS, -1);
        for (int i = 0; i < n; ++i) {
            uint32_t evs = events[i].events;
            int fd = events[i].data.fd;
            if (fd == listenfd) {
                connfd = acceptConn(epollfd, listenfd, events[i]);
                if (addEpollEvent(epollfd, connfd) == -1) {
                    return -1;
                }
                setSoBuffSize(connfd, SO_SNDBUF, BUFSIZE);
                setSoBuffSize(connfd, SO_RCVBUF, BUFSIZE);

                int sendBufSize = getSoBuffSize(connfd, SO_SNDBUF);
                std::cout << "sendBufSize: " << sendBufSize << std::endl;

                int recvBufSize = getSoBuffSize(connfd, SO_RCVBUF);
                std::cout << "recvBufSize: " << recvBufSize << std::endl;
                continue;
            }

            if (evs & EPOLLOUT) {
                // std::cout << "EPOLLOUT" << std::endl;
            }
            if (evs & EPOLLIN) {
                // std::cout << "EPOLLIN" << std::endl;
                recvSync(fd, msg, msgLen);
                sendSync(fd, msg, 128);
                // int ret = recvAsync(fd, msg + recved, msgLen - recved);
                // recved += ret;
                // if (recved == msgLen) {
                //     sendAsync(fd, msg, 128);
                //     recved = 0;
                // }
            }
            if (evs & EPOLLERR) {
                std::cout << "EPOLLERR" << std::endl;
            }
            if (evs & EPOLLHUP) {
                std::cout << "EPOLLHUP" << std::endl;
            }
        }
    }
    close(listenfd);
    return 0;
}
