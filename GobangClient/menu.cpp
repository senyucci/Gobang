#include "menu.h"
#include "ui_menu.h"

Menu::Menu(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Menu)
{
    ui->setupUi(this);

    boundaryWidth=4;    //设置触发resize的宽度
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);     // 设置无边框

    gamecontrol = new GameControl;

    connect(ui->LocalGame_btn,&QPushButton::clicked,this,&Menu::OnLocalGameBtnClick);   // 绑定本地对战按钮
    connect(ui->AiGame_btn,&QPushButton::clicked,this,&Menu::OnAiGameBtnClick);         // 绑定AI对战按钮
    connect(ui->NetGame_btn,&QPushButton::clicked,this,&Menu::OnNetGameBtnClick);       // 绑定联机对战按钮
    connect(gamecontrol,&GameControl::ReturnToMenu,this,[=]()   // 绑定 GameControl 中的返回信号
    {
        gamecontrol->close();
        this->show();
    },Qt::QueuedConnection);
}

Menu::~Menu()
{
    delete ui;
}

void Menu::OnLocalGameBtnClick()
{
    this->hide();
    gamecontrol->InitGame(HUMUN_MODE);  // 以人人对战进行游戏对局
    gamecontrol->show();
}

void Menu::OnAiGameBtnClick()
{
    this->hide();
    gamecontrol->InitGame(AI_MODE);     // 以AI对战进行游戏对局
    gamecontrol->show();
}

void Menu::OnNetGameBtnClick()          // 以联机对战进行游戏对局
{
    NetGame* netgame = new NetGame;
    connect(netgame,&NetGame::ReturnToMenu,this,[=]()   // 联机对战关闭
    {
        netgame->clientSocket->close();
        netgame->close();
        this->show();
    });

    this->hide();
    netgame->InitGame();
    netgame->show();
}


/*************** 重写下方三个事件函数恢复窗体的拖动关闭等操作 ***************/

bool Menu::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = (MSG*)message;
    switch(msg->message)
    {
    case WM_NCHITTEST:
        int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
        int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();
        if(xPos < boundaryWidth && yPos<boundaryWidth)                    //左上角
            *result = HTTOPLEFT;
        else if(xPos>=width()-boundaryWidth&&yPos<boundaryWidth)          //右上角
            *result = HTTOPRIGHT;
        else if(xPos<boundaryWidth&&yPos>=height()-boundaryWidth)         //左下角
            *result = HTBOTTOMLEFT;
        else if(xPos>=width()-boundaryWidth&&yPos>=height()-boundaryWidth)//右下角
            *result = HTBOTTOMRIGHT;
        else if(xPos < boundaryWidth)                                     //左边
            *result =  HTLEFT;
        else if(xPos>=width()-boundaryWidth)                              //右边
            *result = HTRIGHT;
        else if(yPos<boundaryWidth)                                       //上边
            *result = HTTOP;
        else if(yPos>=height()-boundaryWidth)                             //下边
            *result = HTBOTTOM;
        else              //其他部分不做处理，返回false，留给其他事件处理器处理
            return false;
        return true;
    }
    return false;         //此处返回false，留给其他事件处理器处理
}

void Menu::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
        clickPos=e->pos();
}
void Menu::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::LeftButton)
        move(e->pos()+pos()-clickPos);
}


