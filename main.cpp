#include <iostream>
#include <errno.h>
#include "main.h"

extern Monster mob_list[3];

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
    GameStateResponse *gRes = new GameStateResponse();
    gRes->WriteProtocol(ErrorCode::None, game.gstate);
    
    for(UserObject *user : game.users)
    {
        TcpSocket *socket = el.LoadSocket(user->fd);
        if(socket == nullptr) continue;

        int ret = socket->SendSocket(gRes->buffer, gRes->offset);
        if(ret == C_ERR)
        {
            std::cout<<"Failed Send Socket"<<std::endl;
        }
    }

    delete gRes;
}

void GameServer::SendBattleFin()
{
    BattleFinResponse *res = new BattleFinResponse();
    res->WriteProtocol(ErrorCode::None); 
    for(UserObject *user : game.users)
    {
        TcpSocket *socket = el.LoadSocket(user->fd);
        if(socket == nullptr) continue;

        int ret = socket->SendSocket(res->buffer, res->offset);
        if(ret == C_ERR)
        {
            std::cout<<"Failed Send Socket"<<std::endl;
        }
    }

    delete res;
}

// 초기 시작 정보 전송
void GameServer::SendBattleStart()
{
    BattleStartResponse *res = new BattleStartResponse();
    res->WriteProtocol(ErrorCode::None, game.users); 
    for(UserObject* user : game.users)
    {
        TcpSocket *socket = el.LoadSocket(user->fd); 
        if(socket == nullptr) continue; 

        int ret = socket->SendSocket(res->buffer, res->offset);
        if(ret == C_ERR)
        {
            std::cout<<"Failed Send Socket"<<std::endl;
        }
    }

    delete res;
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
                    if(user->has_skill_request == false)
                    {
                        std::cout<<"몬스터들의 기술이 적용되지 않았습니다."<<std::endl;
                        return;
                    }
                }

                std::cout<<"몬스터 기술 계산"<<std::endl;
                CalculatorMonster();

                for(UserObject *user : game.users)
                {
                    user->has_skill_request = false;
                }

                game.round += 1;
            }

            break;
        }

        default:
            break;
    }
}

void GameServer::CalculatorMonster()
{

}

GameServer::GameServer()
{

}

// socket도 제거
GameServer::~GameServer()
{

}