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

void BattleFinResponse::WriteProtocol(ErrorCode r, uuid_t w)
{
    type = ServerMsg::BattleFinResult;
    len = sizeof(ErrorCode) + sizeof(GameState);
    result = r; 
    state = BattleState::Fin;
    win_uid = w;

    memcpy(buffer, &type, sizeof(type));
    offset += sizeof(type);
    memcpy(buffer+offset, &len, sizeof(len));
    offset += sizeof(len);
    memcpy(buffer+offset, &result, sizeof(result));
    offset += sizeof(result);
    memcpy(buffer+offset, &state, sizeof(state));
    offset += sizeof(state);
    memcpy(buffer+offset, &win_uid, sizeof(win_uid));
    offset += sizeof(win_uid);
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

int SkillRequest::ReadProtocol(uint8_t *buffer)
{
    memcpy(&type, buffer, sizeof(type));
    offset += sizeof(type);
    memcpy(&len, buffer+offset, sizeof(len));
    offset += sizeof(len);
    memcpy(&code, buffer+offset, sizeof(code));
    offset += sizeof(code);
    return offset;
}

void SkillResponse::WriteProtocol(ErrorCode r)
{
    type = ServerMsg::SkillResult; 
    len = sizeof(ErrorCode); 
    result = r; 

    memcpy(buffer, &type, sizeof(type));
    offset += sizeof(type);
    memcpy(buffer+offset, &len, sizeof(len));
    offset += sizeof(len);
    memcpy(buffer+offset, &result, sizeof(result));
    offset += sizeof(result);
}

void BattleInfoResponse::WriteProtocol(ErrorCode r, BattleInfo *b, std::vector<UserObject*> users)
{
    type = ServerMsg::BattleInfoResult;
    len = 0; 
    result = r;
    bi = b;

    memcpy(buffer, &type, sizeof(type));
    offset += sizeof(type);
    memcpy(buffer+offset, &len, sizeof(len));
    offset += sizeof(len);
    memcpy(buffer+offset, &result, sizeof(result));
    offset += sizeof(result);
    
    memcpy(buffer+offset, &bi->whois_first, sizeof(bi->whois_first));
    offset += sizeof(bi->whois_first);

    memcpy(buffer+offset, &bi->a_uid, sizeof(bi->a_uid));
    offset += sizeof(bi->a_uid);
    memcpy(buffer+offset, &bi->a_damage, sizeof(bi->a_damage));
    offset += sizeof(bi->a_damage);
    memcpy(buffer+offset, &bi->a_tech, sizeof(bi->a_tech));
    offset += sizeof(bi->a_tech);
    memcpy(buffer+offset, &bi->a_ishit, sizeof(bi->a_ishit));
    offset += sizeof(bi->a_ishit);

    memcpy(buffer+offset, &bi->b_uid, sizeof(bi->b_uid));
    offset += sizeof(bi->b_uid);
    memcpy(buffer+offset, &bi->b_damage, sizeof(bi->b_damage));
    offset += sizeof(bi->b_damage);
    memcpy(buffer+offset, &bi->b_tech, sizeof(bi->b_tech));
    offset += sizeof(bi->b_tech);
    memcpy(buffer+offset, &bi->b_ishit, sizeof(bi->b_ishit));
    offset += sizeof(bi->b_ishit);
}