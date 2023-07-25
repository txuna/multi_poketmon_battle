
all: server_launcher

server_launcher: main.o sock.o event_epoll.o controller.o protocol.o model.o
	g++ -g -o server_launcher -g main.o sock.o event_epoll.o controller.o protocol.o model.o

main.o: main.h main.cpp
	g++ -c -g -o main.o main.cpp

sock.o: sock.h sock.cpp 
	g++ -c -g -o sock.o sock.cpp

event_epoll.o: event_epoll.h event_epoll.cpp	
	g++ -c -g -o event_epoll.o event_epoll.cpp

controller.o: controller.h controller.cpp
	g++ -c -g -o controller.o controller.cpp

protocol.o: protocol.h protocol.cpp 
	g++ -c -g -o protocol.o protocol.cpp

model.o: model.h model.cpp
	g++ -c -g -o model.o model.cpp

clean:
	rm *.o server_launcher