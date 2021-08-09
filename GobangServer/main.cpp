#include "gobangserver.h"

int main()
{
    GobangServer Server;

    Server.InitServer();
    Server.Handler();

    return 0;
}