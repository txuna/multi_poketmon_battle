#include "controller.h"

extern Monster mob_list[3];

Controller::Controller()
{
    
}

Controller::~Controller()
{

}

LoginResponse* LoginController::Login(TcpSocket *socket, int *rb, GameObject *game)
{
    uint8_t *buffer = socket->LoadBuffer();
    ErrorCode result = ErrorCode::None;
    LoginRequest *loginReq = new LoginRequest();
    *rb = loginReq->ReadProtocol(buffer);

    std::cout<<"Request Login "<<socket->GetSocket()<<" : "<<loginReq->name<<std::endl;
    
    result = VerifyUserName(socket, loginReq, game);

    if(result != ErrorCode::None)
    {
        std::cout<<"Failed Login"<<std::endl;
    }
    else
    {
        std::cout<<"Login!"<<std::endl;
        UserObject * user = CreateUser(socket, loginReq);
        game->users.push_back(user);
        //std::cout<<game->users.size()<<std::endl;
    }
    
    LoginResponse *loginRes = new LoginResponse();
    loginRes->WriteProtocol(result, socket->GetSocket());
    
    delete loginReq;
    return loginRes;
}

// 동일한 fd가 있는지 nickname이 있는지 확인
ErrorCode LoginController::VerifyUserName(TcpSocket *socket, LoginRequest *loginReq, GameObject *game)
{
    int fd = socket->GetSocket();

    for(UserObject *user : game->users)
    {
        if(user->fd == fd)
        {
            return ErrorCode::AlreadyLogin;
        }

        if(strcmp((char*)user->name, (char*)loginReq->name) == 0)
        {
            return ErrorCode::AlreadyHasName;
        }
    }

    if(game->users.size() >= MAX_USER)
    {
        return ErrorCode::FullMember;
    }

    return ErrorCode::None;
}

UserObject *LoginController::CreateUser(TcpSocket *socket, LoginRequest *loginReq)
{
    UserObject *user = new UserObject();
    memcpy(user->name, loginReq->name, NAME_LEN-1);
    user->name[NAME_LEN-1] = '\0';
    user->fd = socket->GetSocket();

    return user;
}

// 이미 플레이어가 선택했는지
// 유저 목록에 있는지 확인
ErrorCode ChoiceMonsterController::Verify(UserObject *user, int code)
{
    if(user->mob.code != -1)
    {
        return ErrorCode::AlreadyChoiceMonster;
    }

    int size = sizeof(mob_list) / sizeof(Monster);
    for(int i=0; i<size; i++)
    {
        Monster mob = mob_list[i]; 
        if(mob.code == code)
        {
            return ErrorCode::None;
        }
    }

    return ErrorCode::NonExistMonster;
}

MonsterChoiceResponse *ChoiceMonsterController::Choice(TcpSocket *socket, int *rb, GameObject *game)
{
    uint8_t *buffer = socket->LoadBuffer(); 
    ErrorCode result = ErrorCode::None;
    MonsterChoiceRequest *req = new MonsterChoiceRequest(); 
    *rb = req->ReadProtocol(buffer);

    std::cout<<"Request Choice Monster "<<socket->GetSocket()<<" : "<<req->code<<std::endl;
    UserObject *user = game->LoadUser(socket->GetSocket());
    if(user == nullptr)
    {
        result = ErrorCode::NonExistUser;
    }
    else
    {
        result = Verify(user, req->code); 
        if(result == ErrorCode::None)
        {
            user->mob.Set(req->code);
            std::cout<<"Ok Choice Monster"<<std::endl;
        }
    }

    MonsterChoiceResponse *res = new MonsterChoiceResponse();
    res->WriteProtocol(result);

    delete req;
    return res;
}

// 해당 몬스터에게 존재하는 skill인지 확인
ErrorCode SkillController::Verify(UserObject *user, int code)
{
    if(user->has_skill_request == true)
    {
        return ErrorCode::AlreadyChoiceSkill;
    }

    Monster mob = mob_list[user->mob.code];
    for(int i=0; i<4; i++)
    {
        if(code == mob.tech[i])
        {
            return ErrorCode::None;
        }
    }

    return ErrorCode::InvalidSkill;
}

SkillResponse* SkillController::Choie(TcpSocket *socket, int *rb, GameObject *game)
{
    ErrorCode result = ErrorCode::None;
    uint8_t *buffer = socket->LoadBuffer();
    SkillRequest *req = new SkillRequest();
    *rb = req->ReadProtocol(buffer);

    std::cout<<"Request Skill "<<socket->GetSocket()<<" : "<<req->code<<std::endl;
    UserObject *user = game->LoadUser(socket->GetSocket());

    if(user == nullptr)
    {
        result = ErrorCode::NonExistUser;
    }
    else
    {
        result = Verify(user, req->code);
        if(result == ErrorCode::None)
        {
            user->has_skill_request = true;
        }
    }

    SkillResponse *res = new SkillResponse();
    res->WriteProtocol(result); 

    delete req;
    return res;
}