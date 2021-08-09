/********************************************************************************* 
  *Copyright(C): Seny
  *FileName: command
  *Author: Seny
  *Date: 2021/7/6
  *Description: the command
**********************************************************************************/  
#ifndef _COMMAND_H_
#define _COMMAND_H_

#define ERRPR -1

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



#define ERROR -1
#define MAX_CLIENT 2
#define MAX_CONNEXTIONS 50
#define BUFFER_SIZE 1024

#define HOST "xx.xxx.xx.xxx"
#define PORT 7654


#endif