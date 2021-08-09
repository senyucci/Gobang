/**************************************************************************************
  *Copyright(C): www.seny.xyz
  *FileName: netboard
  *Author: Seny
  *Date: 2021/7/7
  *Description: 联机游戏棋盘上的所有操作包括判定、落子、切换黑白方等,判定均发送至服务器进行
***************************************************************************************/

#ifndef NETBOARD_H
#define NETBOARD_H

#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTcpSocket>
#include <QtNetwork>
#include <QPoint>
#include <QPainter>
#include <QPen>
#include <QVector>
#include <QStack>
#include <QSet>
#include <string.h>
#include "command.h"

namespace Ui {
class NetBoard;
}

struct Data
{
    int dataType;       // 消息类型
    int piece_color;    // 棋手颜色
    int piece_x;        // 棋子坐标
    int piece_y;        // 棋子坐标
    std::string temp;   // 信息摇摆位
};

class NetBoard : public QWidget
{
    Q_OBJECT

public:
    explicit NetBoard(QWidget *parent = 0);
    ~NetBoard();

    // 棋盘所接受的下棋方，人机对战中仅接受白方落子，双人对战中接受黑白双方落子
    void SetBoardReceivePlayer(const QSet<int> &value);
    void SetMousePos(const QPoint &pos);        // 更新当前鼠标位置
    void paintEvent(QPaintEvent *event);        // 绘图事件
    void mouseReleaseEvent(QMouseEvent *event); // 鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event);    // 鼠标移动事件

    Ui::NetBoard *ui;
    void SwitchNextPlayer();                // 切换棋手
    void CheckWinner();                     // 判断赢家
    bool IsFivePiece(int x,int y);          // 判断是否五子连珠
    bool IsVerFivePiece(int x,int y);       // 判断垂直方向五子连珠
    bool IsHorFivePiece(int x,int y);       // 判断水平方向五子连珠
    bool IsNorthWestFivePiece(int x,int y); // 判断45°方向上五子连珠
    bool IsNorthEastFivePiece(int x,int y); // 判断135°方向上五子连珠

signals:
    void GameOver(int winner);              // 游戏结束的信号
    void SetPieceToHost(std::string str);   // 服务器落子信号
    void SendGameOverToHost(int winner);    // 使 NetGame 向服务器发送游戏结束信息的信号

public slots:
    void StartNewGame();                    // 清除棋盘信息，开始新游戏
    void SetPiece(int x,int y);             // 落子
    void Undo(int steps);                   // 悔棋

public:
    static const QSize BOARD_SIZE;          // 棋盘大小
    static const QSize PIECE_SIZE;          // 棋子大小
    static const QPoint START_POS;          // 棋盘中单元格起始位置
    int player;                             // 当前棋盘的棋手


    bool isGameOver;                        // 游戏的结束标志
    int board[BOARD_COL][BOARD_ROW];        // 棋盘中棋子的位置信息
    int nextPlayer;                         // 标志下一位棋手
    QPoint mousePos;                        // 保存鼠标当前位置
    QVector<QPoint> winPiecePos;            // 保存获胜方五个棋子的位置
    QSet<int> boardReceivePlayer;           // 保存棋盘可接受的下棋方
    QStack<QPoint> dropedPieces;            // 保存落子的顺序信息

    Data data;
    std::string dataPacket;


};

void DataToString(std::string &dataPacket,Data &data);           // Data 格式转换为char* 数据包
void StringToData(std::string &dataPacket,Data &data);           // cFhar* 数据包转换为 Data 格式
void BufferClear(std::string &buffer);
void BufferClear(Data &data);

#endif // NETBOARD_H
