/********************************************************************************* 
  *Copyright(C): www.seny.xyz
  *FileName: gobangserver
  *Author: Seny
  *Date: 2021/7/6
  *Description: A Gobang Server for remote game
**********************************************************************************/
#ifndef _GOBANGSERVER_
#define _GOBANGSERVER_
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <fcntl.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stack>
#include "command.h"
#include "locker.h"

using namespace std;

class GobangServer;

struct Data // data form
{
    int dataType;
    int piece_color;
    int piece_x;
    int piece_y;
    string temp;    // 摇摆信息位
};


struct ARG	// to get pThis (static thread function)
{
	GobangServer *pThis;
};

class GobangServer
{
public:
    GobangServer();
    ~GobangServer() {}
    void InitServer();
    void Handler();
    void SendConnectionsToAll();
    void SendDisconnectMsgToAll(int player);
    void DataToChar(char* str,Data &data);
    void CharToData(char* str,Data &data);
    static void* Contact(void *arg);

private:
    // 数据通信相关
    int Server_fd;
    Data data[MAX_CLIENT];
    pthread_t tid[MAX_CLIENT];
    int client_fd[MAX_CLIENT];
    Locker locker;
    int connections;

    // 棋盘相关
    bool player[MAX_CLIENT];                    // 已准备的用户
    int nextPlayer;                             // 下一位落子的用户
};

void BufferClear(char* buffer);
void BufferClear(Data& data);

#endif