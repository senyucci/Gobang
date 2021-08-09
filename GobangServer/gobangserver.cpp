#include "gobangserver.h"

GobangServer::GobangServer()
{
}

void GobangServer::InitServer()
{
    cout << "Server is initializing..." << endl;
    Server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ERROR == Server_fd)
        perror("socket"), exit(0);

    // 端口复用
    int on = 1;
    setsockopt(Server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    setsockopt(Server_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&on, sizeof(on));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(Server_fd, (sockaddr *)&addr, sizeof(addr)) == ERROR)
        perror("bind"), exit(0);

    if (listen(Server_fd, MAX_CONNEXTIONS) == ERROR)
        perror("listen"), exit(0);

    // 初始化fd, 连接数等通信数据
    client_fd[BLACK_PLAYER] = -1;
    client_fd[WHITE_PLAYER] = -1;
    player[BLACK_PLAYER] = false;
    player[WHITE_PLAYER] = false;
    connections = 0;

    cout << "Server is initialized." << endl;
}

void GobangServer::Handler()
{
    // 创建 arg 对象存储 this 指针
    ARG *arg = new ARG();
    arg->pThis = this;

    cout << "waiting for client connect..." << endl;
    for (;;)
    {
        sockaddr_in cAddr;
        socklen_t len = sizeof(cAddr);

        int c_fd = accept(Server_fd, (sockaddr *)&cAddr, &len);
        if (ERROR == c_fd)
        {
            perror("accept");
            close(c_fd);
            continue;
        }

        //  检查连接的合法性
        char tempStr[BUFFER_SIZE];
        Data tmpData;
        BufferClear(tempStr);
        if (read(c_fd, tempStr, BUFFER_SIZE) > 0)
        {
            CharToData(tempStr, tmpData);
            if (tmpData.dataType == CONNECT && connections < MAX_CLIENT) // 若连接通过合法性检验
            {
                int color = (client_fd[BLACK_PLAYER] == -1) ? BLACK_PLAYER : WHITE_PLAYER;
                tmpData.dataType = CONNECT_SUCCESS;
                tmpData.piece_color = color; // 为该客户端分配棋子
                DataToChar(tempStr, tmpData);
                write(c_fd, tempStr, BUFFER_SIZE);

                client_fd[color] = c_fd;
                cout << "client is connect" << endl;

                pthread_create(&tid[color], 0, Contact, (void *)arg); // 创建通信线程
                continue;
            }
            else
            {
                close(c_fd);
                continue;
            }
        }
    }
}

void *GobangServer::Contact(void *arg)
{
    pthread_detach(pthread_self());

    char recvData[BUFFER_SIZE] = {0};
    char sendData[BUFFER_SIZE] = {0};

    // 静态成员函数中获取 this 指针
    ARG *args = (ARG *)arg;
    GobangServer *pThis = args->pThis;

    // 获取该线程所分配的棋子颜色
    int color = (pThis->player[BLACK_PLAYER] == false) ? BLACK_PLAYER : WHITE_PLAYER;

    if (color == BLACK_PLAYER)
        pThis->player[BLACK_PLAYER] = true;
    else
        pThis->player[WHITE_PLAYER] = true;

    int c_fd = pThis->client_fd[color];

    // 更新当前已连接人数
    pThis->locker.lock();
    pThis->connections++;
    pThis->SendConnectionsToAll();
    pThis->locker.unlock();

    cout << "my color is " << ((color == BLACK_PLAYER) ? "black" : "white") << endl;
    cout << c_fd << endl;

    // 控制对局的 Handler
    for (;;)
    {
        BufferClear(recvData);
        if (read(c_fd, recvData, BUFFER_SIZE) > 0)
        {
            pThis->CharToData(recvData, pThis->data[color]); // 转换为 Data 数据
            Data tmpData = pThis->data[color];
            switch (tmpData.dataType)
            {
            case SETPIECE: // 客户端传来落子信息
            {
                if (pThis->connections == MAX_CLIENT)
                {
                    memcpy(sendData, recvData, BUFFER_SIZE);
                    for (int i = 0; i < MAX_CLIENT; i++)
                    {
                        write(pThis->client_fd[i], sendData, BUFFER_SIZE);
                    }
                    cout << "Set " << ((color == BLACK_PLAYER) ? "Black-piece" : "White-piece")
                         << " on (" << tmpData.piece_x << "," << tmpData.piece_y << ")" << endl;
                }
                break;
            }
            case UNDO: // 悔棋请求
            {
                int command = stoi(tmpData.temp); // String 转 int 型 Command
                switch (command)
                {
                case UNDO_REQUEST: // 悔棋二级指令：接收悔棋请求，将请求转发至另一个客户端
                {
                    memcpy(sendData, recvData, BUFFER_SIZE);
                    int otherColor = (color == BLACK_PLAYER) ? WHITE_PLAYER : BLACK_PLAYER;
                    write(pThis->client_fd[otherColor], sendData, BUFFER_SIZE);
                    cout << ((color == BLACK_PLAYER) ? "Black " : "White ") << "player wants to UNDO." << endl;
                    break;
                }
                case UNDO_YES: // 悔棋二级指令：同意悔棋请求，转发至所有客户端
                {
                    memcpy(sendData, recvData, BUFFER_SIZE);
                    for (int i = 0; i < MAX_CLIENT; i++)
                        write(pThis->client_fd[i], sendData, BUFFER_SIZE);
                    cout << ((tmpData.piece_color == BLACK_PLAYER) ? "Black " : "White ") << "player accept to UNDO." << endl;
                    break;
                }
                case UNDO_NO: // 悔棋二级指令：拒绝悔棋请求，转发至所有客户端
                {
                    memcpy(sendData, recvData, BUFFER_SIZE);
                    for (int i = 0; i < MAX_CLIENT; i++)
                        write(pThis->client_fd[i], sendData, BUFFER_SIZE);
                    cout << ((tmpData.piece_color == BLACK_PLAYER) ? "Black " : "White ") << "player refuse to UNDO." << endl;
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case MESSAGE: // 转发聊天信息
            {
                BufferClear(sendData);
                memcpy(sendData, recvData, BUFFER_SIZE);
                for (int i = 0; i < MAX_CLIENT; i++)
                    write(pThis->client_fd[i], sendData, BUFFER_SIZE);
                break;
            }
            case SURRENDER: // 转发投降信息
            {
                BufferClear(sendData);
                memcpy(sendData, recvData, BUFFER_SIZE);
                for (int i = 0; i < MAX_CLIENT; i++)
                    write(pThis->client_fd[i], sendData, BUFFER_SIZE);
                break;
            }
            case TIE:   // 处理求和信息
            {
                int command = stoi(tmpData.temp); // String 转 int 型 Command
                switch (command)
                {
                case TIE_REQUEST: // 求和二级指令：接收求和请求，将请求转发至另一个客户端
                {
                    memcpy(sendData, recvData, BUFFER_SIZE);
                    int otherColor = (color == BLACK_PLAYER) ? WHITE_PLAYER : BLACK_PLAYER;
                    write(pThis->client_fd[otherColor], sendData, BUFFER_SIZE);
                    cout << ((color == BLACK_PLAYER) ? "Black " : "White ") << "player wants to TIE." << endl;
                    break;
                }
                case TIE_YES: // 求和二级指令：同意求和请求，转发至所有客户端
                {
                    memcpy(sendData, recvData, BUFFER_SIZE);
                    for (int i = 0; i < MAX_CLIENT; i++)
                        write(pThis->client_fd[i], sendData, BUFFER_SIZE);
                    cout << ((tmpData.piece_color == BLACK_PLAYER) ? "Black " : "White ") << "player accept to TIE." << endl;
                    break;
                }
                case TIE_NO: // 求和二级指令：拒绝求和请求，转发至所有客户端
                {
                    memcpy(sendData, recvData, BUFFER_SIZE);
                    for (int i = 0; i < MAX_CLIENT; i++)
                        write(pThis->client_fd[i], sendData, BUFFER_SIZE);
                    cout << ((tmpData.piece_color == BLACK_PLAYER) ? "Black " : "White ") << "player refuse to TIE." << endl;
                    break;
                }
                default:
                    break;
                }
            }
            case GAMEOVER: // 游戏结束信息
            {
                cout << "GameOver: " << ((tmpData.piece_color == BLACK_PIECE) ? "black " : "white ") << "player win." << endl;
                break;
            }
            default:
                break;
            }
        }
        else
        {
            cout << "client is disconnect" << endl;

            // 更新当前已连接人数
            pThis->locker.lock();
            pThis->connections--;
            pThis->SendConnectionsToAll();
            pThis->locker.unlock();

            // 释放资源
            close(c_fd);
            pThis->client_fd[color] = -1;
            pThis->player[color] = false;

            // 发送离线信息
            pThis->SendDisconnectMsgToAll(color);

            pthread_exit(NULL);
        }
    }
}

void GobangServer::SendConnectionsToAll()
{
    char sendData[BUFFER_SIZE] = {0};
    Data tmpData;

    BufferClear(tmpData);
    tmpData.dataType = CONNECTIONS;
    tmpData.temp += to_string(connections);
    DataToChar(sendData, tmpData);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (client_fd[i] != -1)
        {
            write(client_fd[i], sendData, BUFFER_SIZE);
        }
    }
}

void GobangServer::SendDisconnectMsgToAll(int player)
{
    char sendData[BUFFER_SIZE] = {0};
    Data tmpData;

    BufferClear(tmpData);
    tmpData.dataType = DISCONNECT;
    tmpData.piece_color = player;
    DataToChar(sendData, tmpData);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (client_fd[i] != -1)
        {
            write(client_fd[i], sendData, BUFFER_SIZE);
        }
    }
}

void GobangServer::CharToData(char *str, Data &data) // 将 str 转换为 Data 格式存储在 data 中
{
    BufferClear(data);
    string tmpData[NUM_OF_MSGTYPE];
    int cnt = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] != ';')
        {
            tmpData[cnt] += str[i];
        }
        else
        {
            cnt++;
        }
        if (cnt == NUM_OF_MSGTYPE)
            break;
    }
    data.dataType = stoi(tmpData[0]);
    data.piece_color = stoi(tmpData[1]);
    data.piece_x = stoi(tmpData[2]);
    data.piece_y = stoi(tmpData[3]);
    data.temp = tmpData[4];
}

void GobangServer::DataToChar(char *str, Data &data) // 将 Data 转为 char* 数据包格式存储在 str 中
{
    BufferClear(str);
    string res;
    string tmpData[NUM_OF_MSGTYPE];

    tmpData[0] = to_string(data.dataType);
    tmpData[1] = to_string(data.piece_color);
    tmpData[2] = to_string(data.piece_x);
    tmpData[3] = to_string(data.piece_y);
    tmpData[4] = data.temp;

    for (int i = 0; i < NUM_OF_MSGTYPE; i++)
    {
        res += tmpData[i];
        res += ';';
    }
    strncpy(str, res.c_str(), BUFFER_SIZE);
}

void BufferClear(char *buffer)
{
    memset(buffer, 0, BUFFER_SIZE);
}

void BufferClear(Data &data)
{
    data.dataType = -1;
    data.piece_color = -1;
    data.piece_x = -1;
    data.piece_x = -1;
    data.temp.clear();
}
