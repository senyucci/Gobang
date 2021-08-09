#include "gamecontrol.h"
#include "ui_gamecontrol.h"

GameControl::GameControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameControl)
{
    ui->setupUi(this);
    ai = new GameAI(this);

    // 美化界面
    boundaryWidth=4;    //设置触发resize的宽度
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);     // 设置无边框

    // 初始化游戏界面
    InitWidget();
}

void GameControl::InitWidget()
{
    // 初始化控件
    blackTimerId = new QTimer(this);
    whiteTimerId = new QTimer(this);

    // 初始化信号与槽
    connect(blackTimerId,&QTimer::timeout,this,&GameControl::BlackTimerUpdate,Qt::QueuedConnection);
    connect(whiteTimerId,&QTimer::timeout,this,&GameControl::WhiteTimerUpdate,Qt::QueuedConnection);
    connect(ui->chessboard,&ChessBoard::TurnToNextPlayer,this,&GameControl::SwitchTimer,Qt::QueuedConnection);
    connect(ui->chessboard,&ChessBoard::GameOver,this,&GameControl::GameOver,Qt::QueuedConnection);
    connect(ui->chessboard,&ChessBoard::TurnToNextPlayer,this,&GameControl::AiNextDropPiece,Qt::QueuedConnection);
    connect(this,&GameControl::GameIsOver,this,&GameControl::GameOver,Qt::QueuedConnection);
    connect(ui->Undo_btn,&QPushButton::clicked,this,&GameControl::Undo);
    connect(ui->Return_btn,&QPushButton::clicked,this,[=]()
    {
        emit(ReturnToMenu());
    });
}

void GameControl::InitGame(int MODE)
{
    // 初始化倒计时
    blackCountdown = COUNTDOWN;         // 初始化倒计时时间
    whiteCountdown = COUNTDOWN;
    whiteTimerId->stop();               // 定时器暂停
    blackTimerId->stop();
    ui->Black_timer->display("10:00");  // 初始化LCD显示
    ui->White_timer->display("10:00");
    SwitchGameMode(MODE);               // 切换游戏模式
}

void GameControl::SwitchTimer(int player)
{
    if(player == BLACK_PLAYER)
    {
        whiteTimerId->stop();       // 白棋定时器停止
        blackTimerId->start(1000);  // 黑棋定时器启动
    }
    else
    {
        blackTimerId->stop();       // 黑棋定时器停止
        whiteTimerId->start(1000);  // 白棋定时器启动
    }
}

void GameControl::BlackTimerUpdate()
{
    // 将时间 String 显示在 LCD_NUM 组件上，Timer 计时1秒后发送timeout信号，TimerUpdate接受到timeout信号后更新倒计时时间
    QString timeString = QString("%1:%2").arg(blackCountdown/60,2,10,QChar('0')).arg(blackCountdown%60,2,10,QChar('0'));
    ui->Black_timer->display(timeString);
    if(!blackCountdown)
    {
         blackCountdown = COUNTDOWN;    // 初始化倒计时时间
         whiteCountdown = COUNTDOWN;
         emit(GameIsOver(WHITE_PLAYER));
    }
    else
    {
         blackCountdown--;
    }
}

void GameControl::WhiteTimerUpdate()
{
    // 将时间 String 显示在 LCD_NUM 组件上，Timer 计时1秒后发送timeout信号，TimerUpdate接受到timeout信号后更新倒计时时间
    QString timeString = QString("%1:%2").arg(whiteCountdown/60,2,10,QChar('0')).arg(whiteCountdown%60,2,10,QChar('0'));
    ui->White_timer->display(timeString);
    if(!whiteCountdown)
    {
         blackCountdown = COUNTDOWN;    // 初始化倒计时时间
         whiteCountdown = COUNTDOWN;
         emit(GameIsOver(BLACK_PLAYER));
    }
    else
    {
        whiteCountdown--;
    }
}

void GameControl::GameOver(int winner)
{
    if(winner != DRAW)  // 若接收到非和棋信号
    {
        QString playerName = (winner == BLACK_PLAYER) ? "黑方" : "白方";
        QMessageBox::information(this, "游戏结束", tr("恭喜%1获胜！！").arg(playerName), QMessageBox::Ok);
        emit(ReturnToMenu());
    }
    else
    {
        QMessageBox::information(this, "游戏结束", "和棋！", QMessageBox::Ok);
        emit(ReturnToMenu());
    }
}

void GameControl::Undo()
{
    if(isAiGame)
    {
        ui->chessboard->Undo(AI_MODE);  // AI悔2步
    }
    else
    {
        ui->chessboard->Undo(HUMUN_MODE);   // Humen悔1步
    }
}

void GameControl::AiNextDropPiece(bool player)  // AI 接受信号后落子
{
    if(isAiGame && player == GameAI::AI_PLAYER)
    {
        QPoint pos = ai->SearchGoodPos(ui->chessboard->GetBoard());
        ui->chessboard->SetPiece(pos.x(),pos.y());
    }
}

void GameControl::SwitchGameMode(int MODE)  // 切换游戏模式
{
    if(MODE == HUMUN_MODE)  // 如果当前是人人对战模式
    {
        QSet<int> boardReceivePlayers;
        boardReceivePlayers << WHITE_PLAYER<< BLACK_PLAYER;     // 棋盘可接受黑白双方下棋
        ui->chessboard->SetBoardReceivePlayer(boardReceivePlayers);
        isHumenGame = true;
        isAiGame = false;
        ui->chessboard->StartNewGame();
    }
    else    // 若当前是 AI 对战模式
    {
        QSet<int> boardReceivePlayers;
        boardReceivePlayers << WHITE_PLAYER;        // 棋盘仅接受白棋
        ui->chessboard->SetBoardReceivePlayer(boardReceivePlayers);
        isHumenGame = false;
        isAiGame = true;
        ui->chessboard->StartNewGame();
    }
}
/*************** 重写下方三个事件函数恢复窗体的拖动关闭等操作 ***************/

bool GameControl::nativeEvent(const QByteArray &eventType, void *message, long *result)
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

void GameControl::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton)
        clickPos=e->pos();
}
void GameControl::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::LeftButton)
        move(e->pos()+pos()-clickPos);
}


GameControl::~GameControl()
{
    delete ui;
}
