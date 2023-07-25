#include "model.h"

Monster mob_list[3] = {
    {0, 10, 10, 10, 10},
    {1, 10, 10, 10, 10},
    {2, 10, 10, 10, 10},
};

void Monster::Init()
{
    code = -1;
    hp = 0; 
    attack = 0;
    defence = 0;
    speed = 0;
}

void Monster::Set(int code)
{
    Monster temp = mob_list[code];
    this->code = temp.code; 
    this->hp = temp.hp; 
    this->attack = temp.attack; 
    this->defence = temp.defence; 
    this->speed = temp.speed;
}


void GameObject::Init()
{
    for(UserObject *user : users)
    {
        std::cout<<"Init Batttle"<<std::endl;
        user->mob.Init();
        user->has_tech_request = false;
    }
    
    round = 1;
    bstate = BattleState::Choice;
}

UserObject* GameObject::LoadUser(socket_t fd)
{
    auto it = std::find_if(users.begin(), users.end(), 
                    [fd](UserObject *user) {
                        return user->fd == fd;
                    });

    if(it != users.end())
    {
        UserObject *user = *it; 
        return user;
    }
    
    return nullptr;
}