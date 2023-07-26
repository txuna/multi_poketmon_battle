#include <iostream>
#include <errno.h>
#include "main.h"

extern Monster mob_list[3];
extern Tech tech_list[4];

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cout<<"./server_launcher [port]"<<std::endl;
        return 1;
    }

    GameServer *game = new GameServer();
    SockAddr *addr = new SockAddr(atoi(argv[1]));
    TcpSocket *socket = new TcpSocket(addr, EPOLLIN | EPOLLOUT);

    if(SetupServer(socket) == false)
    {
        return 1;
    }

    if(game->GameLoop(socket) == C_ERR)
    {
        perror("GameLoop()");
        return 1;
    }

    delete game;
    return 0;
}

int GameServer::GameLoop(TcpSocket *socket)
{
    if(el.CreateEventLoop() == C_ERR)
    {
        perror("CreateEventLoop()");
        return C_ERR;
    }

    if(el.AddEvent(socket) == C_ERR)
    {
        perror("AddEvent()");
        return C_ERR;
    }

    while(isRunning)
    {
        int retval = el.PollEvent();
        ProcessEvent(retval);
    }

    return C_OK;
}

void GameServer::ProcessEvent(int retval)
{
    // Processing Client Input
    for(int i=0; i<retval; i++)
    {
        ev_t e = el.fired[i];
        // need accept or read data from client using socket_fd
        TcpSocket *socket = el.LoadSocket(e.fd);
        // accept
        switch(socket->type)
        {
            case SERVER:
                if(ProcessingAccept(socket, e.mask) == C_ERR)
                {
                    continue;
                }
                break;
            
            case CLIENT:
                ProcessingClientInput(socket, e.mask);
                break;

            default:
                break;
        }
    }

    // Processing Server State 
    ProcessGameState();
    // Send Game State

    if(retval > 0 && el.fired != nullptr)
    {
        delete[] el.fired;
    }
    
    return;
}

int GameServer::ProcessingAccept(TcpSocket *socket, int mask)
{
    if(mask == EPOLLIN)
    {
        TcpSocket *cSocket = socket->AcceptSocket();
        if(cSocket != nullptr)
        {
            if(el.AddEvent(cSocket) == C_ERR)
            {
                perror("AddEvent()");
                return C_ERR;
            }
        }
    }

    return C_OK;
}

// 프로토콜 파싱후 처리
// 패킷이 짤려서 덜 왔을 떄 어떻게 처리할것인가
void GameServer::ProcessingClientInput(TcpSocket *socket, int mask)
{
    if(mask == EPOLLIN)
    {
        int ret = socket->ReadSocket();
        if(ret == C_ERR)
        {
            DeleteGameUser(socket->GetSocket());
            // Close Client
            el.DelEvent(socket);
            return;
        }
        else if(ret == C_YET)
        {
            return;
        }
        
        uint8_t *buffer = socket->LoadBuffer();
        if(buffer[0] != ClientMsg::JoinServer)
        {
            if(AuthUser(socket->GetSocket()) == false)
            {
                return;
            }
        }

        ProcessingPorotocol(socket, ret);
    }
    else
    {
        DeleteGameUser(socket->GetSocket());
        el.DelEvent(socket);
        return;
    }

    return;
}

// game.users에 없다면 false
bool GameServer::AuthUser(socket_t fd)
{
    auto it = std::find_if(game.users.begin(), game.users.end(), 
                           [fd](UserObject *user) {
                                return user->fd == fd;
                           });

    if(it != game.users.end())
    {
        return true;
    }
    
    return false;
}

// 클라이언트 쪽에서 소켓 버퍼가 언제 어떻게 보낼지 모르니 패킷이 잘려서 올 수 있음
// 대비 처리 넣어야 됨
// 읽어들인 전체 길이 확인하고 반복문으로 확인
// JoinServer 제외 유저목록에 있는지 확인 - 미들웨어 느낌
void GameServer::ProcessingPorotocol(TcpSocket *socket, int ret)
{
    uint8_t *buffer = socket->LoadBuffer();
    
    int readByte = 0;
    while(readByte < ret)
    {
        int temp = 0;
        uint8_t type = buffer[readByte];

        switch(type)
        {
            case ClientMsg::JoinServer:
            {
                LoginController *loginController = new LoginController();
                LoginResponse *loginRes = loginController->Login(socket, &temp, &game);
                socket->SendSocket(loginRes->buffer, loginRes->offset);

                delete loginController;
                delete loginRes;
                break;
            }

            case ClientMsg::ChoiceMonster:
            {
                ChoiceMonsterController *controller = new ChoiceMonsterController();
                MonsterChoiceResponse *res = controller->Choice(socket, &temp, &game);
                socket->SendSocket(res->buffer, res->offset);

                delete controller; 
                delete res;
                break;
            }

            case ClientMsg::Skill:
            {
                SkillController *controller = new SkillController();
                SkillResponse *res = controller->Choie(socket, &temp, &game);
                socket->SendSocket(res->buffer, res->offset);

                delete controller; 
                delete res;
                break;
            }

            default:
                std::cout<<"Invalid Protocol"<<std::endl;
                break;
        }

        readByte += temp;
    }
}

void GameServer::DeleteGameUser(socket_t fd)
{   
    auto it = std::find_if(game.users.begin(), game.users.end(), 
                           [fd](UserObject *user) {
                                return user->fd == fd;
                           });

    if(it != game.users.end())
    {
        UserObject *user = *it;
        game.users.erase(it);
        delete user;
    }
}

void GameServer::SendGameState()
{
    GameStateResponse *res = new GameStateResponse();
    res->WriteProtocol(ErrorCode::None, game.gstate);
    
    SendData(res->buffer, res->offset);
    delete res;
}

void GameServer::SendBattleFin()
{
    BattleFinResponse *res = new BattleFinResponse();
    res->WriteProtocol(ErrorCode::None, game.win_uid); 
    
    SendData(res->buffer, res->offset);
    delete res;
}

// 초기 시작 정보 전송
void GameServer::SendBattleStart()
{
    BattleStartResponse *res = new BattleStartResponse();
    res->WriteProtocol(ErrorCode::None, game.users); 

    SendData(res->buffer, res->offset);
    delete res;
}

void GameServer::SendBattleInfo(BattleInfo *bi)
{
    BattleInfoResponse *res = new BattleInfoResponse();
    res->WriteProtocol(ErrorCode::None, bi, game.users);

    SendData(res->buffer, res->offset);
    delete res;
}

void GameServer::SendData(uint8_t *buffer, int len)
{
    for(UserObject *user : game.users)
    {
        TcpSocket *socket = el.LoadSocket(user->fd); 
        if(socket == nullptr) continue; 

        int ret = socket->SendSocket(buffer, len);
        if(ret == C_ERR)
        {
            std::cout<<"Failed Send Socket"<<std::endl;
        }
    }
}

void GameServer::ProcessGameState()
{
    switch(game.gstate)
    {
        // Send Room Info
        // check game user size 
        case GameState::GameReady:
        {
            if(game.users.size() == MAX_USER)
            {
                game.gstate = GameState::GamePlaying;
                // game object init
                game.Init();
                SendGameState();
            }
            break;
        }

        case GameState::GamePlaying:
        {
            if(game.users.size() < MAX_USER)
            {
                game.gstate = GameState::GameReady;
                SendGameState();
                break;
            }

            // 모든 유저가 몬스터 선택을 끝마쳤다면 
            if(game.bstate == BattleState::Choice)
            {
                for(UserObject *user : game.users)
                {
                    if(user->mob.code == -1)
                    {
                        return;
                    }
                }
                
                game.bstate = BattleState::Start;
                SendBattleStart();
                break;
            }

            // 게임이 진행중이라면 게임 정보 전송 
            // 몬스터 정보는 딱 한번 보낼까 Choice단계에서 Start단계로 넘어갈 때?
            // 기술들은 2명다 request 날리고 종합 후 response 이때 몬스터 정보랑 선턴, 기술정보 보내기
            // 경기가 끝났다면 SendBattleFin 보내기 
            else if(game.bstate == BattleState::Start)
            {
                for(UserObject *user : game.users)
                {
                    if(user->has_skill_request == false || user->tech_value == -1)
                    {
                        std::cout<<"몬스터들의 기술이 적용되지 않았습니다."<<std::endl;
                        return;
                    }
                }

                std::cout<<"몬스터 기술 계산"<<std::endl;
                BattleInfo *bi = CalculatorMonster();

                for(UserObject *user : game.users)
                {
                    user->has_skill_request = false;
                    user->tech_value = -1;
                }

                SendBattleInfo(bi);
                game.round += 1;

                delete bi;
            }
            // 경기 종료
            else if(game.bstate == BattleState::Fin)
            {
                std::cout<<"FIN"<<std::endl;
                SendBattleFin();
            }

            break;
        }

        default:
            break;
    }
}

BattleInfo *GameServer::CalculatorMonster()
{
    UserObject *A_user = nullptr; 
    UserObject *B_user = nullptr;
    BattleInfo *bi = new BattleInfo();

    // A_user가 선공권
    if(game.users[0]->mob.speed > game.users[1]->mob.speed)
    {   
        A_user = game.users[0];
        B_user = game.users[1];
    }
    else
    {
        A_user = game.users[1];
        B_user = game.users[0];
    }

    Tech A_tech = tech_list[A_user->tech_value];
    Tech B_tech = tech_list[B_user->tech_value];

    Monster *A_mob = &A_user->mob;
    Monster *B_mob = &B_user->mob;

    int A_damage = 0;
    int B_damage = 0;
    int dec = 0;

    A_damage = (int)(A_mob->attack * A_tech.damage  / 100); 
    dec = (int)((float)(B_mob->defence) / (DEFENCE_VALUE + B_mob->defence) * 100);
    A_damage = (int)(A_damage * (100 - dec) / 100); 


    B_damage = (int)(B_mob->attack * B_tech.damage / 100);
    dec = (int)((float)(A_mob->defence) / (DEFENCE_VALUE + A_mob->defence) * 100);
    B_damage = (int)(B_damage * (100 - dec) / 100); 

    //std::cout<<"User ID("<<A_user->fd<<")"<<"->"<<"("<<B_user->fd<<")"<<"=>"<<A_damage<<std::endl;
    //std::cout<<"User ID("<<B_user->fd<<")"<<"->"<<"("<<A_user->fd<<")"<<"=>"<<B_damage<<std::endl;

    // 죽음 확인 - 선턴 A 
    if(B_mob->hp - A_damage <= 0)
    {
        game.bstate = BattleState::Fin;
        game.win_uid = A_user->fd;
    }
    B_mob->hp -= A_damage;

    if(A_mob->hp - B_damage <= 0)
    {
        game.bstate = BattleState::Fin;
        game.win_uid = B_user->fd;
    }
    A_mob->hp -= B_damage;


    bi->whois_first = A_user->fd; 

    bi->a_damage = A_damage; 
    bi->a_tech = A_user->tech_value; 
    bi->a_ishit = 1;
    bi->a_uid = A_user->fd;

    bi->b_damage = B_damage; 
    bi->b_tech = B_user->tech_value;
    bi->b_ishit = 1;
    bi->b_uid = B_user->fd;

    return bi;
}

GameServer::GameServer()
{

}

// socket도 제거
GameServer::~GameServer()
{

}