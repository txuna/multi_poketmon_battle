#ifndef _T_SOCK_H
#define _T_SOCK_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "model.h"
#include "common.h"


class SockAddr
{
    public:
        struct sockaddr_in adr;
        SockAddr(int port); 
        SockAddr(struct sockaddr_in a);
        ~SockAddr();
};


class TcpSocket
{
    private:
        uint8_t buf[SOCKET_BUFFER] = {0};
        int mask;
        socket_t socket_fd;
        SockAddr* sock_addr;
        int available_size = -1;
        
    public:
        int type;
        TcpSocket(SockAddr *adr, int mask, socket_t fd);
        TcpSocket(SockAddr *adr, int mask);
        TcpSocket();
        ~TcpSocket();
        void SetSockAddr(SockAddr *adr);
        int CreateSocket();
        int BindSocket();
        int ListenSocket();
        TcpSocket *AcceptSocket();
        int ReadSocket();
        int SendSocket(uint8_t *buffer, int len);
        socket_t GetSocket();
        int GetMask();
        void SetMask(int mask);
        uint8_t *LoadBuffer();
};

#endif