#include "chessboard.h"
#include "ui_chessboard.h"

const QSize ChessBoard::BOARD_SIZE(430,430);          // 棋盘大小
const QSize ChessBoard::PIECE_SIZE(25,25);            // 棋子大小
const QPoint ChessBoard::START_POS(40,40);            // 棋盘中单元格起始位置

ChessBoard::ChessBoard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChessBoard)
{
    ui->setupUi(this);
    InitBoard();
}

void ChessBoard::InitBoard()
{
    boardReceivePlayer << WHITE_PLAYER << BLACK_PLAYER; // 棋盘默认双人对战
    StartNewGame();
}

void ChessBoard::StartNewGame() // 开始新游戏
{
    for (int i = 0; i < BOARD_COL; i++)
    {
        for (int j = 0; j < BOARD_ROW; j++)
        {
            board[i][j] = NO_PIECE;
        }
    }
    winPiecePos.clear();
    dropedPieces.clear();
    nextPlayer = BLACK_PLAYER;
    isGameOver = false;
    update();
    emit(TurnToNextPlayer(nextPlayer));
}

void ChessBoard::paintEvent(QPaintEvent *event) // 绘图事件
{
    // 设置背景颜色
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Background,QColor("#B1723C")); // 背景色：棕黄色
    this->setPalette((palette));

    // 绘制棋盘边界与棋盘内颜色
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true); // 抗锯齿

    QBrush  brush;
    brush.setColor(QColor("#EEC085"));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.drawRect(20,20,390,390);

    // 绘制棋盘格
    for (int i = 0; i < BOARD_COL - 1; i++)
    {
        for (int j = 0; j < BOARD_ROW - 1; j++)
        {
            painter.drawRect(QRect(START_POS + QPoint(i * PIECE_SIZE.width(), j * PIECE_SIZE.height()),
                                   PIECE_SIZE));
        }
    }

    // 绘制五个小黑点("天元"与"星")
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    painter.drawRect(112,112,6,6);
    painter.drawRect(312,112,6,6);
    painter.drawRect(112,337,6,6);
    painter.drawRect(312,337,6,6);
    painter.drawRect(212,212,6,6);

    // 绘制落子前的红色选框
    painter.setPen(Qt::red);
    QPoint pos[12] = {
        mousePos + QPoint(0, 8),
        mousePos,
        mousePos + QPoint(8, 0),
        mousePos + QPoint(17, 0),
        mousePos + QPoint(25, 0),
        mousePos + QPoint(25, 8),
        mousePos + QPoint(25, 17),
        mousePos + QPoint(25, 25),
        mousePos + QPoint(17, 25),
        mousePos + QPoint(8, 25),
        mousePos + QPoint(0, 25),
        mousePos + QPoint(0, 17)
    };
    painter.drawPolyline(pos, 3);   // 绘制四次平行线
    painter.drawPolyline(pos + 3, 3);
    painter.drawPolyline(pos + 6, 3);
    painter.drawPolyline(pos + 9, 3);

    // 绘制棋子
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < BOARD_COL; i++)
    {
        for (int j = 0; j < BOARD_ROW; j++)
        {
            if (board[i][j] != NO_PIECE)
            {
                QColor color = (board[i][j] == WHITE_PIECE) ? Qt::white : Qt::black;
                painter.setBrush(QBrush(color));
                painter.drawEllipse(START_POS.x() - PIECE_SIZE.width()/2 + i*PIECE_SIZE.width(),
                                    START_POS.y() - PIECE_SIZE.height()/2 + j*PIECE_SIZE.height(),
                                    PIECE_SIZE.width(), PIECE_SIZE.height());
            }
        }
    }

    // 绘制最后一次落子的标志
    painter.setPen(Qt::red);
    if (!dropedPieces.isEmpty())
    {
        QPoint lastPos = dropedPieces.top();
        QPoint drawPos = START_POS + QPoint(lastPos.x() * PIECE_SIZE.width(), lastPos.y() * PIECE_SIZE.height());
        painter.drawLine(drawPos + QPoint(0, 5), drawPos + QPoint(0, -5));
        painter.drawLine(drawPos + QPoint(5, 0), drawPos + QPoint(-5, 0));
    }

    // 绘制五子连珠的标志
    for (QPoint pos : winPiecePos)
    {
        QPoint drawPos = START_POS + QPoint(pos.x() * PIECE_SIZE.width(), pos.y() * PIECE_SIZE.height());
        painter.drawLine(drawPos + QPoint(0, 5), drawPos + QPoint(0, -5));
        painter.drawLine(drawPos + QPoint(5, 0), drawPos + QPoint(-5, 0));
    }
}

void ChessBoard::mouseMoveEvent(QMouseEvent *event) // 鼠标移动事件
{
    // 获取相对坐标
    QPoint pos = event->pos() - START_POS + QPoint(PIECE_SIZE.width()/2,PIECE_SIZE.height()/2);
    int x = pos.x();
    int y = pos.y();

    // 判断鼠标是否在棋盘内
    if(x < 0 || x >= PIECE_SIZE.width() * BOARD_COL || y < 0 || y >= PIECE_SIZE.height() * BOARD_ROW)
        return;

    // 设置偏移量
    int offsetX = x % PIECE_SIZE.width();
    int offsetY = y % PIECE_SIZE.height();

    // 更新已保存的的鼠标位置
    SetMousePos(QPoint(x - offsetX,y - offsetY) + START_POS - QPoint(PIECE_SIZE.width()/2, PIECE_SIZE.height()/2));
}

void ChessBoard::mouseReleaseEvent(QMouseEvent *event) // 鼠标点击事件
{
    if(boardReceivePlayer.contains((nextPlayer) && !isGameOver))    // 选手判断 && 游戏结束判断
    {
        QPoint pos  = event->pos() - START_POS; // 获取点击的相对位置
        int x = pos.x();
        int y = pos.y();

        // 定位点击的格子位置
        int pieceX = x / PIECE_SIZE.width();
        int pieceY = y / PIECE_SIZE.height();
        int offsetX = x % PIECE_SIZE.width();
        int offsetY = y % PIECE_SIZE.height();

        // 判断格子点击时的左右偏移量
        if (offsetX > PIECE_SIZE.width() / 2)
            pieceX++;
        if(offsetY > PIECE_SIZE.height() / 2)
            pieceY++;
        SetPiece(pieceX,pieceY);
    }
}

void ChessBoard::SetMousePos(const QPoint &pos) // 将鼠标在棋盘中的位置保存于 mousePos 中
{
    mousePos = pos;
    update();
}

void ChessBoard::SetPiece(int x,int y)  // 落子
{
    if (x >= 0 && x < BOARD_COL && y >= 0 && y < BOARD_ROW && board[x][y] == NO_PIECE)  // 边界判断 && 无子判断
    {
        dropedPieces.push(QPoint(x, y));
        board[x][y] = (nextPlayer == BLACK_PLAYER) ? BLACK_PIECE : WHITE_PIECE;
        update();
        CheckWinner();  // 胜负判断
        if (!isGameOver)
        {
            SwitchNextPlayer();
        }
    }
}

void ChessBoard::SwitchNextPlayer() // 切换选手
{
    nextPlayer = !nextPlayer;
    emit TurnToNextPlayer(nextPlayer);  // 切换倒计时等
}

void ChessBoard::CheckWinner() // 胜负判断
{
    bool fullPieces = true;
    for(int i = 0; i < BOARD_COL;i++)
    {
        for(int j = 0; j < BOARD_ROW;j++)
        {
            if(board[i][j] == NO_PIECE) // 判满
            {
                fullPieces = false;
            }
            if(board[i][j] != NO_PIECE && IsFivePiece(i,j)) // 若有一方达成五子
            {
                bool winner = (board[i][j] == WHITE_PIECE) ? WHITE_PLAYER : BLACK_PLAYER;
                isGameOver = true;
                emit GameOver(winner);
            }
        }
    }
    if (fullPieces)
    {
        isGameOver = true;
        emit GameOver(DRAW);    // 和棋
    }
}

bool ChessBoard::IsFivePiece(int x,int y)   // 判断五子
{
    return IsVerFivePiece(x,y)||IsHorFivePiece(x,y)||IsNorthWestFivePiece(x,y)||IsNorthEastFivePiece(x,y);
}

bool ChessBoard::IsVerFivePiece(int x,int y)    // 判断垂直方向五子
{
    int piece = board[x][y];
    for (int i = 1;i <5;i++)
    {
        if(y + i >= BOARD_ROW || board[x][y+i] != piece)
            return false;
    }
    winPiecePos.clear();
    for(int i = 0;i < 5;i++)
        winPiecePos.append(QPoint(x,y+i));
    return true;
}

bool ChessBoard::IsHorFivePiece(int x,int y)    // 水平方向五子
{
    int piece = board[x][y];
    for (int i = 1;i <5;i++)
    {
        if(x + i >= BOARD_COL || board[x+i][y] != piece)
            return false;
    }
    winPiecePos.clear();
    for(int i = 0;i < 5;i++)
        winPiecePos.append(QPoint(x+i,y));

    return true;
}

bool ChessBoard::IsNorthWestFivePiece(int x,int y)  // 判断45°方向五子
{
    int piece = board[x][y];
    for (int i = 1;i <5;i++)
    {
        if(x + i >= BOARD_COL || y-i < 0|| board[x+i][y-i] != piece)
            return false;
    }
    winPiecePos.clear();
    for(int i = 0;i < 5;i++)
        winPiecePos.append(QPoint(x+i,y-i));

    return true;
}

bool ChessBoard::IsNorthEastFivePiece(int x,int y)  // 判断135°方向五子
{
    int piece = board[x][y];
    for (int i = 1;i <5;i++)
    {
        if(x + i >= BOARD_COL || y-i >= BOARD_ROW|| board[x+i][y+i] != piece)
            return false;
    }
    winPiecePos.clear();
    for(int i = 0;i < 5;i++)
        winPiecePos.append(QPoint(x+i,y+i));

    return true;
}

void ChessBoard::Undo(int steps)    // 悔棋
{
    if(!isGameOver)
    {
        for(int i =0 ;i < steps && !dropedPieces.isEmpty();i++)
        {
            QPoint pos = dropedPieces.pop();
            board[pos.x()][pos.y()] = NO_PIECE;
        }
        update();
        if(steps == AI_MODE)
        {
            nextPlayer = BLACK_PLAYER;
        }
        SwitchNextPlayer();
    }
}

void ChessBoard::SetBoardReceivePlayer(const QSet<int> &value)  // 设置棋盘可接受的棋手
{
    boardReceivePlayer = value;
}

BoardInfo ChessBoard::GetBoard()
{
    return board;
}


ChessBoard::~ChessBoard()
{
    delete ui;
}
