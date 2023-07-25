#ifndef _T_USER_H
#define _T_USER_H

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>

#include "common.h"

class Monster 
{
    public:
        int code;
        int hp; 
        int attack; 
        int defence; 
        int speed; 

        void Init();
        void Set(int code);
};

class UserObject
{
    public:
        uint8_t name[NAME_LEN];
        socket_t fd;
        Monster mob;
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