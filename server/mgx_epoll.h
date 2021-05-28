#include <sys/epoll.h>
#include <map>
using namespace std;

static map<int,string> clients;

class MgxEpoll
{

#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

public:

    //IO多路复用epoll
    static void do_epoll(int listenfd);
    //事件处理函数
    static void
    handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char *buf);
    //处理接收到的连接
    static void handle_accpet(int epollfd, int listenfd);
    //读处理
    static void do_read(int epollfd, int fd, char *buf);
    //写处理
    static void do_write(int epollfd, int fd, char *buf);
    //添加事件
    static void add_event(int epollfd, int fd, int state);
    //修改事件
    static void modify_event(int epollfd, int fd, int state);
    //删除事件
    static void delete_event(int epollfd, int fd, int state);
};
