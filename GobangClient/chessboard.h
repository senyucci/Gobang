/*********************************************************************************************
  *Copyright(C): www.seny.xyz
  *FileName: chessboard
  *Author: Seny
  *Date: 2021/7/3
  *Description: 本地游戏棋盘上的所有操作包括判定、落子、切换黑白方等，通过一个棋手集合来控制落子方
**********************************************************************************************/

#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QPen>
#include <QVector>
#include <QStack>
#include <QSet>
#include "command.h"


namespace Ui {
class ChessBoard;
}

typedef int (*BoardInfo)[15];

class ChessBoard : public QWidget
{
    Q_OBJECT

public:
    explicit ChessBoard(QWidget *parent = 0);
    ~ChessBoard();

    // 棋盘所接受的下棋方，人机对战中仅接受白方落子，双人对战中接受黑白双方落子
    void SetBoardReceivePlayer(const QSet<int> &value);

    BoardInfo GetBoard();                       // AI模式中获取棋盘信息
    void SetMousePos(const QPoint &pos);        // 更新当前鼠标位置
    void paintEvent(QPaintEvent *event);        // 绘图事件
    void mouseReleaseEvent(QMouseEvent *event); // 鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event);    // 鼠标移动事件

private:
    Ui::ChessBoard *ui;
    void InitBoard();                       // 初始化棋盘信息
    void SwitchNextPlayer();                // 切换棋手
    void CheckWinner();                     // 判断赢家
    bool IsFivePiece(int x,int y);          // 判断是否五子连珠
    bool IsVerFivePiece(int x,int y);       // 判断垂直方向五子连珠
    bool IsHorFivePiece(int x,int y);       // 判断水平方向五子连珠
    bool IsNorthWestFivePiece(int x,int y); // 判断45°方向上五子连珠
    bool IsNorthEastFivePiece(int x,int y); // 判断135°方向上五子连珠

signals:
    void GameOver(int winner);              // 游戏结束的信号
    void TurnToNextPlayer(int player);      // 切换棋手的信号

public slots:
    void StartNewGame();                    // 清除棋盘信息，开始新游戏
    void SetPiece(int x,int y);             // 落子
    void Undo(int steps);                   // 悔棋

public:
    static const QSize BOARD_SIZE;          // 棋盘大小
    static const QSize PIECE_SIZE;          // 棋子大小
    static const QPoint START_POS;          // 棋盘中单元格起始位置

private:
    bool isGameOver;                        // 游戏的结束标志
    int board[BOARD_COL][BOARD_ROW];        // 棋盘中棋子的位置信息
    int nextPlayer;                         // 标志下一位棋手
    QPoint mousePos;                        // 保存鼠标当前位置
    QVector<QPoint> winPiecePos;            // 保存获胜方五个棋子的位置
    QSet<int> boardReceivePlayer;           // 保存棋盘可接受的下棋方
    QStack<QPoint> dropedPieces;            // 保存落子的顺序信息
};

#endif // CHESSBOARD_H
