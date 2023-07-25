#include "protocol.h"


int Protocol::ReadProtocol()
{
    return 0;
}

void Protocol::WriteProtocol()
{

}

int LoginRequest::ReadProtocol(uint8_t *buffer)
{
    memcpy(&type, buffer, sizeof(type));
    offset += sizeof(type);
    memcpy(&len, buffer+offset, sizeof(len));
    offset += sizeof(len);
    memcpy(name, buffer+offset, len);
    offset += len;

    return offset;
}

void LoginResponse::WriteProtocol(ErrorCode r, uuid_t u)
{
    type = ServerMsg::JoinResult;
    len = sizeof(ErrorCode);
    result = r;
    uid = u;

    memcpy(buffer, &type, sizeof(type));
    offset += sizeof(type);
    memcpy(buffer+offset, &len, sizeof(len));
    offset += sizeof(len);
    memcpy(buffer+offset, &result, len);
    offset += sizeof(result);
    memcpy(buffer+offset, &uid, len); 
    offset += sizeof(uid);
}

void GameStateResponse::WriteProtocol(ErrorCode r, GameState s)
{
    type = ServerMsg::GameStateResult;
    len = sizeof(ErrorCode) + sizeof(GameState);
    result = r; 
    state = s;

    memcpy(buffer, &type, sizeof(type));
    offset += sizeof(type);
    memcpy(buffer+offset, &len, sizeof(len));
    offset += sizeof(len);
    memcpy(buffer+offset, &result, sizeof(result));
    offset += sizeof(result);
    memcpy(buffer+offset, &state, sizeof(state));
    offset += sizeof(state);
}

int MonsterChoiceRequest::ReadProtocol(uint8_t *buffer)
{
    memcpy(&type, buffer, sizeof(type));
    offset += sizeof(type);
    memcpy(&len, buffer+offset, sizeof(len));
    offset += sizeof(len);
    memcpy(&code, buffer+offset, sizeof(code));
    offset += sizeof(code);
    return offset;
}

void MonsterChoiceResponse::WriteProtocol(ErrorCode r)
{
    type = ServerMsg::ChoiceMonsterResult; 
    len = sizeof(ErrorCode); 
    result = r; 

    memcpy(buffer, &type, sizeof(type));
    offset += sizeof(type);
    memcpy(buffer+offset, &len, sizeof(len));
    offset += sizeof(len);
    memcpy(buffer+offset, &result, sizeof(result));
    offset += sizeof(result);
}

void BattleStateResponse::WriteProtocol(ErrorCode r, BattleState s)
{
    type = ServerMsg::BattleStateResult;
    len = sizeof(ErrorCode) + sizeof(GameState);
    result = r; 
    state = s;

    memcpy(buffer, &type, sizeof(type));
    offset += sizeof(type);
    memcpy(buffer+offset, &len, sizeof(len));
    offset += sizeof(len);
    memcpy(buffer+offset, &result, sizeof(result));
    offset += sizeof(result);
    memcpy(buffer+offset, &state, sizeof(state));
    offset += sizeof(state);
}

void BattleStartResponse::WriteProtocol(ErrorCode r, std::vector<UserObject*> users)
{
    type = ServerMsg::BattleStartResult; 
    len = sizeof(ErrorCode) + (sizeof(uuid_t) * 2) + (sizeof(int) * 2);
    result = r;

    memcpy(buffer, &type, sizeof(type));
    offset += sizeof(type);
    memcpy(buffer+offset, &len, sizeof(len));
    offset += sizeof(len);
    memcpy(buffer+offset, &result, sizeof(result));
    offset += sizeof(result);
    
    for(UserObject *user : users)
    {
        memcpy(buffer+offset, &user->fd, sizeof(user->fd));
        offset += sizeof(user->fd);

        memcpy(buffer+offset, &user->mob.code, sizeof(user->mob.code));
        offset += sizeof(user->mob.code);
    }
}

void BattleInfoResponse::WriteProtocol(ErrorCode r)
{

}