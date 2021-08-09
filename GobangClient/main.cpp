#include "menu.h"
#include "netgame.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Menu menu;
    menu.show();

    return a.exec();
}
