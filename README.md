# multi_poketmon_battle
포켓몬 형식의 1대1 멀티 배틀게임입니다. 

# Tech Stack 
- Client  
Godot 4.0 

- Server  
C++ 

# Architecture 
서버의 경우 C++을 중심으로 epoll api를 사용하여 설계했습니다. 

![logic](./images/logic.png)


# Image 

### Login  
![login](./images/login.png)

### Waiting Other Player 
![waiting](./images/waiting.png)

### Choice Monster 
![choose](./images/choose.png)

### Play!
![play](./images/play.png)

### Fin!
![fin](./images/fin.png)