/***************************************************************************
  *Copyright(C): www.seny.xyz
  *FileName: menu
  *Author: Seny
  *Date: 2021/7/4
  *Description: 主菜单相关界面
****************************************************************************/

#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>
#include "gamecontrol.h"
#include "netgame.h"

namespace Ui {
class Menu;
}

class Menu : public QMainWindow
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = 0);
    ~Menu();

    // 界面美化：恢复无边框后的窗体移动事件
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void InitMenu();                    // 初始化主菜单界面

public slots:
    void OnLocalGameBtnClick();         // 本地对战槽函数
    void OnAiGameBtnClick();            // 人机对战槽函数
    void OnNetGameBtnClick();           // 联机对战槽函数

private:
    Ui::Menu *ui;
    GameControl* gamecontrol;           // 本地对局控件


    int boundaryWidth;
    QPoint clickPos;



};

#endif // MENU_H
