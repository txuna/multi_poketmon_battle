#ifndef _T_PROTOCOL_H
#define _T_PROTOCOL_H

#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>

#include "common.h"
#include "model.h"

/*
Protocol Form 
len은 type, len을 제외한 data payload의 길이 

1. Login Request
type(1byte)
len(4byte)
name(32byte)

2. Login Response 
type(1byte)
len(4byte)
result(4byte)
uid(4byte)

3. GameState Response 
type(1byte)
len(4byte)
result(4byte)
state(4byte)

4. MosnterChoice Request 
type(1byte)
len(4byte)
code(4byte)

5. MonsterChoice Response 
type(1byte)
len(4byte)
result(4byte)

6. BattleStart Response
type(1byte)
len(4byte)
result(4byte)

name(32byte)
code(4byte)

name(32byte)
code(4byte)

7. skill request 
type(1byte)
len(4byte)
skill_id 

8. skill response 
type(1byte)
len(4byte)
result(4byte)

*/

class Protocol
{
    public:
        uint8_t type; 
        uint32_t len; 
        int offset = 0;
        int ReadProtocol();
        void WriteProtocol();
};


class LoginRequest : public Protocol
{
    public:
        uint8_t name[NAME_LEN] = {0};
        int ReadProtocol(uint8_t *buffer);
};

class LoginResponse : public Protocol
{
    public:
        ErrorCode result;
        uuid_t uid;
        uint8_t buffer[SOCKET_BUFFER] = {0};
        void WriteProtocol(ErrorCode r, uuid_t u);
};

class GameStateResponse : public Protocol
{
    public:
        ErrorCode result;
        GameState state;
        uint8_t buffer[SOCKET_BUFFER] = {0};
        void WriteProtocol(ErrorCode r, GameState s);
};

class MonsterChoiceRequest : public Protocol
{
    public:
        int code; 
        int ReadProtocol(uint8_t *buffer);
};

class MonsterChoiceResponse : public Protocol
{
    public:
        ErrorCode result;
        uint8_t buffer[SOCKET_BUFFER] = {0};
        void WriteProtocol(ErrorCode r);
};

class BattleFinResponse : public Protocol
{
    public:
        ErrorCode result; 
        BattleState state; 
        uuid_t win_uid;
        uint8_t buffer[SOCKET_BUFFER] = {0};
        void WriteProtocol(ErrorCode r, uuid_t w);
};

class BattleStartResponse : public Protocol
{
    public:
        uint8_t buffer[SOCKET_BUFFER] = {0};
        ErrorCode result; 
        uuid_t a_uid; 
        int a_code;

        uuid_t b_uid; 
        int b_code;

        void WriteProtocol(ErrorCode r, std::vector<UserObject*> users);
};

class BattleInfoResponse : public Protocol
{
    public:
        uint8_t buffer[SOCKET_BUFFER] = {0};
        ErrorCode result; 
        BattleInfo *bi;

        void WriteProtocol(ErrorCode r, BattleInfo *b, std::vector<UserObject*> users);
};

class SkillRequest : public Protocol
{
    public:
        int code; 
        int ReadProtocol(uint8_t *buffer);
};

class SkillResponse : public Protocol
{
    public:
        uint8_t buffer[SOCKET_BUFFER] = {0};
        ErrorCode result;
    
        void WriteProtocol(ErrorCode r);
};



#endif 