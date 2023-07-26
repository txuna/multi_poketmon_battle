#ifndef _T_USER_H
#define _T_USER_H

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>

#include "common.h"

class Tech
{
    public:
        int code;
        int type;
        int damage; 
        int hit_rate;
};

class Monster 
{
    public:
        int code;
        int hp; 
        int attack; 
        int defence; 
        int speed; 
        int type;
        int tech[4];

        void Init();
        void Set(int code);
};

class UserObject
{
    public:
        uint8_t name[NAME_LEN];
        socket_t fd;
        Monster mob;
        // 서버는 지속적으로 user목록을 돌면서 해당 유저 모두가 해당 변수값으로 세팅되었다면 포켓몬 기술 계산 후 응답 전송
        bool has_skill_request; // 기술을 서버에 보냈는지 해당 라운드가 끝날때 false로 다시 세팅
};


class GameObject
{
    public:
        int round = 1;
        GameState gstate = GameState::GameReady;
        BattleState bstate = BattleState::Choice; 
        std::vector<UserObject*> users;
        
        void Init();
        UserObject *LoadUser(socket_t fd);
};

#endif