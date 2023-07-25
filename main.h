#ifndef _T_MAIN_H
#define _T_MAIN_H

#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>

#include "sock.h"
#include "event_epoll.h"
#include "protocol.h"
#include "controller.h"
#include "common.h"

static bool SetupServer(TcpSocket *socket)
{
    if(socket->CreateSocket() == C_ERR)
    {
        perror("CreateSocket()");
        return false;
    }

    if(socket->BindSocket() == C_ERR)
    {
        perror("BindSocket()");
        return false;
    }

    if(socket->ListenSocket() == C_ERR)
    {
        perror("ListenSocket()");
        return false;
    }

    return true;
}

class GameServer
{
    private:
        bool isRunning = true;
        int roomIndex = 12;

    public:
        GameObject game;
        EventLoop el;
        int GameLoop(TcpSocket *socket);
        void ProcessEvent(int retval);
        void SendGameState();
        // Choice, Start, Fin 기존과 다르게 변경되었을 때 알림
        void SendBattleFin();
        // 게임정보 전송
        void SendBattleStart();
        void SendBattleInfo();
        bool CheckChoiceMonster();
        void ProcessGameState();
        int ProcessingAccept(TcpSocket *socket, int mask);
        void ProcessingClientInput(TcpSocket *socket, int mask);
        void ProcessingPorotocol(TcpSocket *socket, int ret);
        void DeleteGameUser(socket_t fd);
        bool AuthUser(socket_t fd);
        void CalculatorMonster();
        GameServer();
        ~GameServer();
};

#endif 