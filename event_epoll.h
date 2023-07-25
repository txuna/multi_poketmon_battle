#ifndef _T_EPOLL_H
#define _T_EPOLL_H

#include "sock.h"
#include <sys/epoll.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <algorithm>

#define MAX_EVENT_SIZE 1024
#define REDUNDANCY_SIZE 12

struct ev_t
{
    int fd; 
    int mask;
};

class EventLoop
{
    private:
        int epfd;

    public:
        std::vector<TcpSocket*> events;
        ev_t *fired = nullptr; //epoll_wait를 통해서 생긴 것들
        
        EventLoop();
        int CreateEventLoop();
        int AddEvent(TcpSocket *tcpSocket);
        int DelEvent(TcpSocket *tcpSocket);
        TcpSocket *LoadSocket(socket_t fd);
        int PollEvent();
        ~EventLoop();
};

#endif