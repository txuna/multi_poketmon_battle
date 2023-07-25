#include "event_epoll.h"


EventLoop::EventLoop()
{
    epfd = -1;
}

int EventLoop::CreateEventLoop()
{
    if(epfd != -1)
    {
        return C_ERR; // already create event loop
    }

    epfd = epoll_create(MAX_EVENT_SIZE);
    if(epfd == -1)
    {
        return C_ERR;
    }

    return C_OK;
}

TcpSocket *EventLoop::LoadSocket(socket_t fd)
{
    auto it = std::find_if(events.begin(), events.end(), 
                           [fd](TcpSocket *socket) {
                                return socket->GetSocket() == fd;
                           });

    if(it != events.end())
    {
        TcpSocket *socket = *it; 
        return socket;
    }
    
    return nullptr;
}

int EventLoop::DelEvent(TcpSocket *tcpSocket)
{
    int fd = tcpSocket->GetSocket();
    int mask = tcpSocket->GetMask();
    struct epoll_event ee = {0};
    ee.data.fd = fd;
    ee.events = mask;
    epoll_ctl(epfd, EPOLL_CTL_DEL, ee.data.fd, &ee);

    auto it = std::find_if(events.begin(), events.end(), 
                           [fd](TcpSocket *socket) {
                                return socket->GetSocket() == fd;
                           });
    if(it != events.end())
    {
        TcpSocket *rmsocket = *it;
        events.erase(it);
        delete rmsocket;
    }
    else
    {
        return C_ERR;
    }
    return C_OK;
}

int EventLoop::AddEvent(TcpSocket *tcpSocket)
{
    struct epoll_event ee = {0};   
    int fd = tcpSocket->GetSocket();
    int mask = tcpSocket->GetMask();

    ee.events = 0;
    ee.events = mask; 
    ee.data.fd = fd;
    if(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ee) == -1)
    {
        return C_ERR;
    }

    events.push_back(tcpSocket);
    return C_OK;
}

int EventLoop::PollEvent()
{
    struct epoll_event ees[MAX_EVENT_SIZE];
    struct timeval tvp;
    tvp.tv_sec = 0;
    tvp.tv_usec = 100;
    int retval = epoll_wait(epfd, ees, MAX_EVENT_SIZE, tvp.tv_usec);
    
    if(retval <= 0)
    {
        return retval;
    }

    fired = new struct ev_t[retval];

    for(int i=0; i<retval; i++)
    {
        struct epoll_event *e = (ees + i);
        fired[i].fd = e->data.fd;
        fired[i].mask = e->events; 
    }

    return retval;
}

EventLoop::~EventLoop()
{
    if(epfd != -1)
    {
        close(epfd);
    }

    for(TcpSocket *socket : events)
    {
        delete socket;
    }

    events.clear();
}