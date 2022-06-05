#include <arpa/inet.h>
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define PORT 9999
#define IP "127.0.0.1"

void read_cb(struct bufferevent *bev, void *arg) {
    char buf[1024];
    memset(buf, '\0', sizeof(buf));
    //从缓冲区读取数据
    bufferevent_read(bev, buf, sizeof(buf));
    printf("client send data: %s\n", buf);
    char *p = "i am server, i got you.\n";
    //注意数据向缓冲区写完，才会回调write_cb
    bufferevent_write(bev, p, strlen(p));
}

void write_cb(struct bufferevent *bev, void *arg) { printf("i am server, write data.... done!\n"); }

void event_cb(struct bufferevent *bev, short events, void *arg) {
    if (events & BEV_EVENT_EOF) {
        printf("connection closed\n");
    } else if (events & BEV_EVENT_ERROR) {
        printf("error\n");
    }
    bufferevent_free(bev);
}

void listen_cb(struct evconnlistener *listener, evutil_socket_t sock, struct sockaddr *addr,
               int len, void *ptr) {
    char ip[16];
    memset(&ip, '\0', sizeof(ip));
    struct sockaddr_in *client_addr = (struct sockaddr_in *)&addr;

    // struct sockaddr_in ip_adr_get;
    // int ip_adr_len;

    // ip_adr_len = sizeof(ip_adr_get);
    // getpeername(fd, &ip_adr_get, &ip_adr_len);

    // printf("IP address is: %s\n", inet_ntoa(ip_adr_get.sin_addr));
    // printf("Port is: %d\n", (int)ntohs(ip_adr_get.sin_port));

    printf("receive connection from ip:%s port:%d\n",
           inet_ntop(AF_INET, &client_addr->sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(client_addr->sin_port));

    struct event_base *base = (struct event_base *)ptr;
    struct bufferevent *bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);
    //启用bufferevent读缓冲，（默认创建时，读缓冲处于关闭状态，写缓冲处于开启状态）
    bufferevent_enable(bev, EV_READ);
}

int main() {
    struct event_base *base = event_base_new();

    //初始化地址结构
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    //创建监听服务器
    struct evconnlistener *evl =
        evconnlistener_new_bind(base, listen_cb, base, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                -1, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    //释放资源
    event_base_dispatch(base);
    evconnlistener_free(evl);
    event_base_free(base);
    return 0;
}