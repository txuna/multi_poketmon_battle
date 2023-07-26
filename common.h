#ifndef _T_COMMON_H
#define _T_COMMON_H

typedef int socket_t;
typedef int uuid_t;

#define SOCKET_BUFFER 4096
#define CLIENT 0
#define SERVER 1

#define NAME_LEN 32
#define TITLE_LEN 32

#define C_OK 0
#define C_ERR -1
#define C_YET -2

#define MAX_USER 2

enum AttackType
{
    Fire = 1,
    Water, 
    Grass, 
    Normal
};

enum GameState
{
    GameReady = 1,
    GamePlaying
};

enum BattleState
{  
    Choice, 
    Start, 
    Fin
};

enum ServerMsg
{
    JoinResult = 1,
    GameStateResult,
    ChoiceMonsterResult,
    BattleFinResult,
    BattleStartResult,
    BattleInfoResult, 
    SkillResult,
};

enum ClientMsg
{
    JoinServer = 1,
    ChoiceMonster,
    Skill,
};

enum ErrorCode
{
    None = 1,
    AlreadyLogin,
    AlreadyHasName,
    FullMember,
    NonExistUser,
    AlreadyChoiceMonster,
    NonExistMonster,
    InvalidSkill,
    AlreadyChoiceSkill,
};

#endif 