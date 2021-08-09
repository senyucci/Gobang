/***************************************************************************
  *Copyright(C): www.seny.xyz
  *FileName: gameai
  *Author: Seny
  *Date: 2021/7/4
  *Description: 棋盘上的AI逻辑与落子操作
****************************************************************************/

#ifndef GAMEAI_H
#define GAMEAI_H

#include <QObject>
#include "chessboard.h"

class GameAI : public QObject
{
    Q_OBJECT
public:
    explicit GameAI(QObject *parent = nullptr);

    QVector<QPoint> GetAllDropPiecePos();       // 返回棋盘上所有NO_PIECE位置
    int GetPieceScore(int x,int y,int player);  // 返回该位置相对于player的分数
    int GetPosValue(int x,int y);               // 返回该位置的分数，越靠近中间分数越高
    int GetChessType(int x,int y,int direction,int pieceColor);     // 返回该位置direction方向上的棋形
    int GetLinePieceNum(int x,int y,int direction,int pieceColor,int &pieceEnd);    //获取该位置该方向上pieceColor颜色的连续棋子数，pieceEnd为结束位置棋子
    bool IsBeyond(int x,int y);

public slots:
    QPoint SearchGoodPos(BoardInfo Board);

public:
    static const int CHESS_VALUE[9];
    static const int MIN_VALUE = -2000000;
    static const int AI_PLAYER = BLACK_PLAYER;

private:
    int board[BOARD_COL][BOARD_ROW];
    QPoint bestPos;
};

#endif // GAMEAI_H
