/*********************************************************************************************
  *Copyright(C): www.seny.xyz
  *FileName: netgame
  *Author: Seny
  *Date: 2021/7/7
  *Description: 负责联网游戏对局逻辑：包含一个NetBoard棋盘控件与一个 Client_Socket 与服务器通信
**********************************************************************************************/

#ifndef NETGAME_H
#define NETGAME_H

#include <QWidget>
#include <QTcpSocket>
#include <QtNetwork>
#include <string>
#include <QMouseEvent>
#include <QPushButton>
#include <windowsx.h>
#include <windows.h>
#include "netboard.h"
#include "command.h"

namespace Ui {
class NetGame;
}

class NetGame : public QWidget
{
    Q_OBJECT
public:
    explicit NetGame(QWidget *parent = 0);
    ~NetGame();

    void InitWidget();                     // 初始化窗体
    void InitGame();                       // 初始化游戏
    void InitSocket();                     // 初始化socket连接
    void ReceiveHandler();                 // 接受服务器信息的处理器

    // 界面美化相关
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

public slots:
    void GameOver(int winner);             // 游戏结束相关事件
    void doConnected();                    // 服务器连接
    void doDisconnected();                 // 服务器断开
    void SetPieceOnHost(std::string str);  // 落子
    void SendGameOverOnHost(int winner);   // 发送游戏结束信息给服务器
    void OnUndoBtnClick();                 // 悔棋按钮事件
    void OnSentBtnClick();                 // 发送按钮事件
    void OnSurrenderBtnClick();            // 投降按钮事件
    void OnTieBtnClick();                  // 求和按钮事件

signals:
    void ReturnToMenu();                   // 返回主菜单的信号
    void GameIsOver(int winner);           // 游戏结束的信号

public:
    Ui::NetGame *ui;
    NetBoard * netboard;                   // 联网棋盘控件
    int player;                            // 存储当前客户端分配的棋子颜色
    QString num;                           // 存储当前连接数

    // 数据传输
    QTcpSocket* clientSocket;              // 客户端文件描述符
    std::string dataPacket;                // 转换后的数据包
    Data data;                             // 数据格式

    // 界面美化相关
    int boundaryWidth;
    QPoint clickPos;
};


#endif // NETGAME_H
