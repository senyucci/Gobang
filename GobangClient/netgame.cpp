#include "netgame.h"
#include "ui_netgame.h"

#include <QDebug>

NetGame::NetGame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetGame)
{
    ui->setupUi(this);
    boundaryWidth=4;    //设置触发resize的宽度
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);     // 设置无边框

    // 初始化 TCP 连接
    clientSocket = new QTcpSocket(this);
}

NetGame::~NetGame()
{
    delete ui;
}

void NetGame::InitWidget()
{
    ui->msgRecord->append("服务器未连接");

    // 设置黑白棋子lable可见性
    ui->white_label->setVisible(false);
    ui->black_label->setVisible(false);

    connect(ui->netboard,&NetBoard::SetPieceToHost,this,&NetGame::SetPieceOnHost);
    connect(ui->netboard,&NetBoard::SendGameOverToHost,this,&NetGame::SendGameOverOnHost);
    connect(ui->netboard,&NetBoard::GameOver,this,&NetGame::GameOver);
    connect(ui->undoBtn,&QPushButton::clicked,this,&NetGame::OnUndoBtnClick);
    connect(ui->sendBtn,&QPushButton::clicked,this,&NetGame::OnSentBtnClick);
    connect(ui->surrenderBtn,&QPushButton::clicked,this,&NetGame::OnSurrenderBtnClick);
    connect(ui->tieBtn,&QPushButton::clicked,this,&NetGame::OnTieBtnClick);
    connect(ui->closeBtn,&QPushButton::clicked,this,[=]()
    {
        emit(ReturnToMenu());
    });
}

void NetGame::InitGame()
{
    InitSocket();
    InitWidget();
    ui->netboard->StartNewGame();
}


void NetGame::InitSocket()
{
    clientSocket->abort();
    connect(clientSocket,&QTcpSocket::connected,this,&NetGame::doConnected);
    connect(clientSocket,&QTcpSocket::disconnected,this,&NetGame::doDisconnected);
    connect(clientSocket,&QTcpSocket::readyRead,this,&NetGame::ReceiveHandler);

    clientSocket->connectToHost(HOST,PORT);
}

void NetGame::GameOver(int winner) // 游戏结束相关事件
{

    if(winner != DRAW)  // 若接收到非和棋信号
    {
        QString playerName = (winner == BLACK_PLAYER) ? "黑方" : "白方";
        QMessageBox::information(this, "提示", tr("游戏结束，恭喜%1获胜！！").arg(playerName), QMessageBox::Ok);
        emit(ReturnToMenu());
    }
    else
    {
        QMessageBox::information(this, "提示", "游戏结束，双方和棋！", QMessageBox::Ok);
        emit(ReturnToMenu());

    }
}

void NetGame::doConnected()   // 服务器连接
{
    data.dataType = CONNECT;
    ui->msgRecord->clear();
    ui->msgRecord->append("服务器已连接");
    DataToString(dataPacket,data);

    clientSocket->write(dataPacket.c_str());
}

void NetGame::doDisconnected()   // 服务器断开
{
    ui->msgRecord->append("服务器已断开");
}

void NetGame::ReceiveHandler()   // 消息处理器
{
    QByteArray buffer;
    buffer = clientSocket->readAll();
    BufferClear(dataPacket);
    dataPacket = buffer.data();   // 将buffer中接收到的数据转换为String数据格式
    if(!buffer.isEmpty())
    {
        StringToData(dataPacket,data);  // 将String数据包转换为Data格式
        switch(data.dataType)
        {
        case CONNECT_SUCCESS:   // 连接成功
        {
            player = data.piece_color;
            QString str = (player == BLACK_PLAYER) ? "黑方":"白方";
            str += "已准备";
            ui->msgRecord->append(str.toUtf8());
            if(player == BLACK_PLAYER)
                ui->black_label->setVisible(true);
            else
                ui->white_label->setVisible(true);

            // 棋盘设置服务器分配的棋子
            ui->netboard->player = player;
            QSet<int> boardReceivePlayers;
            boardReceivePlayers << player;
            ui->netboard->SetBoardReceivePlayer(boardReceivePlayers);
            ui->netboard->StartNewGame();
            break;
        }
        case SETPIECE:   // 落子
        {
            int x = data.piece_x;
            int y = data.piece_y;
            ui->netboard->dropedPieces.push(QPoint(x, y));
            ui->netboard->board[x][y] = (data.piece_color == BLACK_PIECE) ? BLACK_PIECE : WHITE_PIECE;
            ui->netboard->update();
            ui->netboard->CheckWinner();    // 胜负判断
            if (!ui->netboard->isGameOver)
            {
                ui->netboard->SwitchNextPlayer();
            }
            break;
        }
        case UNDO:   // 悔棋
        {
            int command = std::stoi(data.temp);     // 获取 UNDO 的分级指令
            switch (command)
            {
            case UNDO_REQUEST:
            {
                int id = QMessageBox::information(NULL,"提示","对方请求悔棋!",QString("同意"),QString("拒绝"),0);
                switch (id) {
                case 0:     // 同意：向服务端发送同意数据包
                {
                    data.temp = std::to_string(UNDO_YES);
                    data.piece_color = player;
                    DataToString(dataPacket,data);
                    clientSocket->write(dataPacket.c_str());
                    break;
                }
                case 1:     // 拒绝：向服务端发送拒绝数据包
                {
                    data.temp = std::to_string(UNDO_NO);
                    data.piece_color = player;
                    DataToString(dataPacket,data);
                    clientSocket->write(dataPacket.c_str());
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case UNDO_YES:
            {
                QString str = (data.piece_color == BLACK_PLAYER) ? "黑方":"白方";
                str += "同意悔棋";
                ui->msgRecord->append(str);
                ui->netboard->Undo(HUMUN_MODE);
                break;
            }
            case UNDO_NO:
            {
                QString str = (data.piece_color == BLACK_PLAYER) ? "黑方":"白方";
                str += "拒绝悔棋";
                ui->msgRecord->append(str);
                break;
            }
            default:
                break;
            }
            break;
        }
        case CONNECTIONS:   // 显示当前连接人数
        {
            num = QString::fromStdString(data.temp);
            ui->connections->setText(num);
            break;
        }
        case MESSAGE:   // 聊天数据包
        {
            int color = data.piece_color;
            QString str = (color == BLACK_PLAYER) ? "黑方: ":"白方: ";
            str += QString::fromStdString(data.temp);
            ui->msgRecord->append(str.toUtf8());        // 显示在消息记录中
            break;
        }
        case SURRENDER: // 投降数据包
        {
            QString playerName = (data.piece_color == BLACK_PLAYER) ? "黑方" : "白方";
            QMessageBox::information(this, "游戏结束", tr("%1已认输！").arg(playerName), QMessageBox::Ok);
            emit(ReturnToMenu());
            break;
        }
        case TIE:   // 求和数据包
        {
            int command = std::stoi(data.temp);     // 获取 TIE(求和) 的分级指令
            switch (command)
            {
            case TIE_REQUEST:   // 对方请求和棋
            {
                int id = QMessageBox::information(NULL,"提示","对方请求和棋!",QString("同意"),QString("拒绝"),0);
                switch (id) {
                case 0:     // 同意：向服务端发送同意数据包
                {
                    data.temp = std::to_string(TIE_YES);
                    data.piece_color = player;
                    DataToString(dataPacket,data);
                    clientSocket->write(dataPacket.c_str());
                    break;
                }
                case 1:     // 拒绝：向服务端发送拒绝数据包
                {
                    data.temp = std::to_string(TIE_NO);
                    data.piece_color = player;
                    DataToString(dataPacket,data);
                    clientSocket->write(dataPacket.c_str());
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case TIE_YES:   // 对方接受和棋
            {
                QMessageBox::information(this, "提示", "游戏结束，双方和棋！", QMessageBox::Ok);
                emit(ReturnToMenu());
                break;
            }
            case TIE_NO:    // 对方拒绝和棋
            {
                QString str = (data.piece_color == BLACK_PLAYER) ? "黑方":"白方";
                str += "拒绝和棋";
                ui->msgRecord->append(str);
                break;
            }
            default:
                break;
            }
            break;
        }
        case DISCONNECT:   // 显示用户离线信息
        {
            QString str = (data.piece_color == BLACK_PLAYER) ? "黑方" : "白方";
            str += "已离开";
            ui->msgRecord->append(str);
            break;
        }
        default:
            break;
        }
    }
}

void NetGame::OnUndoBtnClick()  // 悔棋按钮事件
{
    if(ui->netboard->nextPlayer != player)
    {
        int id = QMessageBox::information(NULL,"提示","确认悔棋",QString("是"),QString("否"),0);
        switch (id) {
        case 0:     // 同意：向服务端发送求和数据包
        {
            std::string tempStr;
            Data tempData;
            tempData.dataType = UNDO;
            tempData.temp = std::to_string(UNDO_REQUEST);    // int 型 Command 转 String 格式
            DataToString(tempStr,tempData);
            clientSocket->write(tempStr.c_str());       // 向服务端发送 UNDO_REQUEST
            break;
        }
        case 1:
        {
            break;
        }
        default:
            break;
        }
    }
}

void NetGame::OnSentBtnClick() // 发送按钮事件
{
    Data tmpData;
    tmpData.dataType = MESSAGE;
    tmpData.piece_color = player;

    QString str = ui->EnterLineEdit->text().toUtf8();   // 将输入框的内容装填入Data数据包内
    tmpData.temp = str.toStdString();

    DataToString(dataPacket,tmpData);
    clientSocket->write(dataPacket.c_str());
    ui->EnterLineEdit->clear();
}

void NetGame::OnSurrenderBtnClick() // 投降按钮事件
{
    Data tmpData;
    int id = QMessageBox::information(NULL,"提示","确认投降",QString("是"),QString("否"),0);
    switch (id) {
    case 0:     // 同意：向服务端发送求和数据包
    {
        tmpData.dataType = SURRENDER;
        tmpData.piece_color = player;
        DataToString(dataPacket,tmpData);
        clientSocket->write(dataPacket.c_str());
        break;
    }
    case 1:
    {
        break;
    }
    default:
        break;
    }
}

void NetGame::OnTieBtnClick()   // 求和按钮事件
{
    Data tmpData;
    int id = QMessageBox::information(NULL,"提示","确认求和",QString("是"),QString("否"),0);
    switch (id) {
    case 0:     // 同意：向服务端发送求和数据包
    {
        tmpData.dataType = TIE;
        tmpData.piece_color = player;
        tmpData.temp = std::to_string(TIE_REQUEST);    // int 型 Command 转 String 格式
        DataToString(dataPacket,tmpData);
        clientSocket->write(dataPacket.c_str());
        break;
    }
    case 1:
    {
        break;
    }
    default:
        break;
    }
}

void NetGame::SetPieceOnHost(std::string str)  // 服务器发送落子命令
{
    clientSocket->write(str.c_str());
}

void NetGame::SendGameOverOnHost(int winner)    // 向服务器发送游戏结束的信息
{
    BufferClear(data);
    data.dataType = GAMEOVER;
    data.piece_color = winner;
    DataToString(dataPacket,data);
    clientSocket->write(dataPacket.c_str());
}



/*************** 重写下方三个事件函数恢复窗体的拖动关闭等操作 ***************/

void NetGame::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton) // 恢复鼠标按压事件
        clickPos=e->pos();
}

void NetGame::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::LeftButton)  // 恢复鼠标移动事件
        move(e->pos()+pos()-clickPos);
}

bool NetGame::nativeEvent(const QByteArray &eventType, void *message, long *result)
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

