#ifndef _T_CONTROLLER_H
#define _T_CONTROLLER_H

#include <stdint.h>
#include <iostream>
#include <vector>
#include "sock.h"
#include "protocol.h"
#include "common.h"

class UserObject;
class RoomObject;

class Controller
{
    public:
        Controller();
        ~Controller();
};

class LoginController : public Controller
{
    public:
        ErrorCode VerifyUserName(TcpSocket *socket, LoginRequest *loginReq, GameObject *game);
        LoginResponse *Login(TcpSocket *socket, int *rb, GameObject *game);
        UserObject *CreateUser(TcpSocket *socket, LoginRequest *loginReq);
};

class ChoiceMonsterController : public Controller
{
    public:
        // 이미 선택했는지, 존재하는 몬스터인지 확인
        ErrorCode Verify(UserObject *user, int code);
        MonsterChoiceResponse *Choice(TcpSocket *socket, int *rb, GameObject *game);
};

class SkillController : public Controller
{
    public:
        ErrorCode Verify(UserObject *user, int code);
        SkillResponse *Choie(TcpSocket *socket, int *rb, GameObject *game);
};

#endif 