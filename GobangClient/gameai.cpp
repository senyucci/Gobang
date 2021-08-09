#include "gameai.h"
#include "cmath"
const int GameAI::CHESS_VALUE[9] = {0, 50, 100, 200, 500, 650, 2500, 10000, 30000};
const int GameAI::MIN_VALUE;
const int GameAI::AI_PLAYER;

GameAI::GameAI(QObject *parent) : QObject(parent)
{

}

QVector<QPoint> GameAI::GetAllDropPiecePos()
{
    QVector<QPoint> allPos;
    for(int i = 0;i < BOARD_COL;i++)
    {
        for(int j = 0;j < BOARD_ROW;j++)
        {
            if (board[i][j] == NO_PIECE)
            {
                allPos.push_back(QPoint(i,j));
            }
        }
    }
    return allPos;
}

int GameAI::GetPieceScore(int x, int y, int player)
{
    int value = 0;
    int piece = (player == WHITE_PLAYER) ? WHITE_PLAYER : BLACK_PLAYER;
    for(int i = 0;i < 8; i++)
    {
        int t = GetChessType(x,y,i,piece);
        value += CHESS_VALUE[t];
    }
    value += GetPosValue(x,y);
    return value;
}

int GameAI::GetPosValue(int x,int y)
{
    int valueX = BOARD_COL - std::abs(x - BOARD_COL / 2);
    int valueY = BOARD_ROW - std::abs(y - BOARD_ROW / 2);
    return valueX + valueY;
}

int GameAI::GetChessType(int x,int y,int direction,int piece)
{
    int chessType = 0;
    int end = NO_PIECE;
    int num = GetLinePieceNum(x,y,direction,piece,end);
    if(num == 5)
    {
        chessType = 8;
    }
    else if(num == 4 && end == NO_PIECE)
    {
        chessType = 7;
    }
    else if (num == 4 && end != NO_PIECE)
    {
        chessType = 6;
    }
    else if (num == 3 && end == NO_PIECE)
    {
        chessType = 5;
    }
    else if (num == 3 && end != NO_PIECE)
    {
        chessType = 4;
    }
    else if (num == 2 && end == NO_PIECE)
    {
        chessType = 3;
    }
    else if (num == 2 && end != NO_PIECE)
    {
        chessType = 2;
    }
    else if (num == 1 && end == NO_PIECE)
    {
        chessType = 1;
    }
    else
    {
        chessType = 0;
    }
    return chessType;
}

int GameAI::GetLinePieceNum(int x, int y, int direction, int pieceColor, int &pieceEnd)
{
    int offset[8][2] = {{1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}};
    int num = 0;
    x += offset[direction][0];
    y += offset[direction][1];
    if(IsBeyond(x,y) || board[x][y] != pieceColor)
    {
        return 0;
    }
    int pieceStart = board[x][y];
    while(!IsBeyond(x,y) && board[x][y] == pieceStart)
    {
        x += offset[direction][0];
        y += offset[direction][1];
        num++;
    }
    pieceEnd = board[x][y];     // 终止处的棋子
    return num;
}

bool GameAI::IsBeyond(int x, int y)
{
    return x < 0 || x >= BOARD_COL || y < 0 || y >=  BOARD_ROW;
}

QPoint GameAI::SearchGoodPos(BoardInfo Board)
{
    memcpy(board,Board,sizeof(board));
    int bestScore = MIN_VALUE;
    for(QPoint pos : GetAllDropPiecePos())
    {
        int value = GetPieceScore(pos.x(),pos.y(),AI_PLAYER);
        int oppoValue = GetPieceScore(pos.x(),pos.y(),!AI_PLAYER);
        int totalValue = std::max(value,oppoValue * 4/5);
        if(totalValue > bestScore)
        {
            bestScore = totalValue;
            bestPos = pos;
        }
    }
    return bestPos;
}

