#include "model.h"

Monster mob_list[3] = {
    {0, 100, 20, 50, 20, AttackType::Water, {0, 2, -1, -1}},
    {1, 100, 25, 50, 15, AttackType::Grass, {0, 3, -1, -1}},
    {2, 100, 30, 50, 10, AttackType::Fire, {0, 1, -1, -1}},
};

Tech tech_list[4] = {
    {0, AttackType::Normal, 30, 100},
    {1, AttackType::Fire, 50, 80},
    {2, AttackType::Water, 40, 100},
    {3, AttackType::Grass, 60, 70},
};

void Monster::Init()
{
    code = -1;
    hp = 0; 
    attack = 0;
    defence = 0;
    speed = 0;
    type = 0;
}

void Monster::Set(int code)
{
    Monster temp = mob_list[code];
    this->code = temp.code; 
    this->hp = temp.hp; 
    this->attack = temp.attack; 
    this->defence = temp.defence; 
    this->speed = temp.speed;
    this->type = temp.type;
}


void GameObject::Init()
{
    for(UserObject *user : users)
    {
        std::cout<<"Init Batttle"<<std::endl;
        user->mob.Init();
        user->has_skill_request = false;
        user->tech_value = -1;
    }
    
    win_uid = -1;
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