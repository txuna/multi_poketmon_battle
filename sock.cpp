#include <iostream>
#include <stdint.h>
#include <errno.h>

#include "sock.h"
#include "event_epoll.h"


SockAddr::SockAddr(int port)
{
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(port);
}

SockAddr::SockAddr(struct sockaddr_in a)
{
    adr = a;
}


SockAddr::~SockAddr()
{

}

TcpSocket::TcpSocket(SockAddr *adr, int mask)
{
    socket_fd = -1;
    sock_addr = adr; 
    this->mask = mask;
}

TcpSocket::TcpSocket(SockAddr *adr, int mask, socket_t fd)
{
    socket_fd = fd;
    sock_addr = adr;
    this->mask = mask;
}

TcpSocket::TcpSocket()
{
    socket_fd == -1;
}

void TcpSocket::SetSockAddr(SockAddr *addr)
{
    sock_addr = addr;
}

int TcpSocket::CreateSocket()
{
    int flags;
    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1)
    {
        return C_ERR;
    }

    int on = 1;
    if(setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        return C_ERR;
    }

    if((flags = fcntl(socket_fd, F_GETFL, 0)) == -1)
    {
        return C_ERR;
    }

    if(fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        return C_ERR;
    }

    type = SERVER;

    return C_OK;
}

int TcpSocket::BindSocket()
{
    if(bind(socket_fd, (struct sockaddr*)&sock_addr->adr, sizeof(sock_addr->adr)) == -1)
    {
        return C_ERR;
    }

    return C_OK;
}

int TcpSocket::ListenSocket()
{
    if(listen(socket_fd, 5) == -1)
    {
        return C_ERR;
    }

    return C_OK;
}

TcpSocket *TcpSocket::AcceptSocket()
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    socket_t fd = accept4(socket_fd, (sockaddr*)&addr, &addr_len, SOCK_NONBLOCK);
    //socket_t fd = accept(socket_fd, (sockaddr*)&addr, &addr_len);
    if(fd <= 0)
    {
        return nullptr;
    }
    SockAddr *sock = new SockAddr(addr);
    TcpSocket *socket = new TcpSocket(sock, EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR, fd);
    socket->type = CLIENT;

    return socket;
}

// when return -1 check EAGAIN 
// if not EAGAIN, Close Socket
int TcpSocket::ReadSocket()
{
    memset(buf, 0, sizeof(buf));
    int ret = read(socket_fd, buf, SOCKET_BUFFER);
    if(ret < 0)
    {
        if(errno == EAGAIN)
        {
            return C_OK;
        }
        return C_ERR;
    }
    if(ret == 0)
    {
        return C_ERR;
    }

    // ret이 SOCKET_BUFFER와 같다는것은 덜 읽었을 가능성 존재 
    if(ret == SOCKET_BUFFER)
    {
        ioctl(socket_fd, FIONREAD, &available_size);
    }
    
    return ret;
}

int TcpSocket::SendSocket(uint8_t *buffer, int len)
{
    int ret = write(socket_fd, buffer, len);
    if(ret < 0) return C_ERR;
    return C_OK;
}

socket_t TcpSocket::GetSocket()
{
    return socket_fd;
}

int TcpSocket::GetMask()
{
    return mask;
}

uint8_t *TcpSocket::LoadBuffer()
{
    return buf;
}

TcpSocket::~TcpSocket()
{
    if(socket_fd != -1)
    {
        close(socket_fd);
    }

    if(sock_addr != nullptr)
    {
        delete sock_addr;
    }
}