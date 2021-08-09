/***************************************************************************
  *Copyright(C): www.seny.xyz
  *FileName: GameControl
  *Author: Seny
  *Date: 2021/7/3
  *Description: 负责本地游戏对局的两种模式：AI模式与双人模式
****************************************************************************/

#ifndef GAMECONTROL_H
#define GAMECONTROL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLCDNumber>
#include <QString>
#include <QChar>
#include <QTimer>
#include <windowsx.h>
#include <windows.h>
#include "chessboard.h"
#include "gameai.h"
#define COUNTDOWN 599                   // 倒计时时间

namespace Ui {
class GameControl;
}

class GameControl : public QWidget
{
    Q_OBJECT

public:
    explicit GameControl(QWidget *parent = 0);
    ~GameControl();

    void InitWidget();                  // 初始化游戏界面
    void InitGame(int MODE);            // 开始新游戏
    void SwitchTimer(int player);       // 切换选手倒计时
    void Undo();                        // 悔棋
    void SwitchGameMode(int MODE);      // 切换人机与人人对战模式

    // 界面美化：恢复无边框后的窗体移动事件
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

signals:
    void ReturnToMenu();                  // 返回主菜单的信号
    void GameIsOver(int winner);          // 游戏结束的信号

public slots:
    void BlackTimerUpdate();            // 更新黑方定时器
    void WhiteTimerUpdate();            // 更新白方定时器
    void GameOver(int winner);          // 游戏结束相关事件
    void AiNextDropPiece(bool player);  // AI模式中接受下棋信号

public:
    Ui::GameControl *ui;
    GameAI *ai;
    QTimer* blackTimerId;               // 黑方定时器
    QTimer* whiteTimerId;               // 白方定时器

    int blackCountdown;                 // 黑方初始倒计时时间
    int whiteCountdown;                 // 白方初始倒计时时间
    bool isHumenGame = true;            // 模式标志：人人对战
    bool isAiGame = false;              // 模式标志：人机对战
    int boundaryWidth;  // 界面美化
    QPoint clickPos;    // 恢复鼠标点击
};

#endif // GAMECONTROL_H
