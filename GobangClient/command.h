/***************************************************************************
  *Copyright(C): www.seny.xyz
  *FileName: command
  *Author: Seny
  *Date: 2021/7/3
  *Description: COMMAND
****************************************************************************/

#ifndef COMMAND_H
#define COMMAND_H

#define BUFFER_SIZE 1024

// 消息类型
#define CONNECT 110
#define CONNECT_SUCCESS 120
#define CONNECTIONS 130
#define SETPIECE 140

#define UNDO 150
#define UNDO_REQUEST 151
#define UNDO_YES 152
#define UNDO_NO 153

#define GAMEOVER 160
#define DISCONNECT 170
#define MESSAGE 180
#define SURRENDER 190

#define TIE 200
#define TIE_REQUEST 201
#define TIE_YES 202
#define TIE_NO 203

#define NUM_OF_MSGTYPE 5

// 棋盘数据
#define BOARD_COL  15        // 棋盘列数
#define BOARD_ROW  15        // 棋盘行数
#define BLACK_PIECE  0       // 棋子标志：黑子
#define WHITE_PIECE  1       // 棋子标志：白子
#define NO_PIECE  2          // 棋子标志：无子
#define BLACK_PLAYER  0      // 棋手标志：黑方
#define WHITE_PLAYER  1      // 棋手标志：白方
#define DRAW 2               // 和棋标志
#define HUMUN_MODE 1         // 悔棋步数：人类 1 步
#define AI_MODE 2            // 悔棋步数：AI 2 步

// 联机服务器
// HOST 填入服务器 IP 地址
#define HOST "xx.xxx.xx.xxx"
#define PORT 7654

#endif // COMMAND_H
