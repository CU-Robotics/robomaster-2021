/*
** client.c -- a stream socket client demo
*/
#include "ClientServer.h"

int main(int argc, const char* argv[])
{
    if(argc != 2) {
        cout << "Usage: pass hostname with argument" << endl;
        return 1;
    }
    Client reciever(argv[1]);
    if(reciever.setup()){ 
        reciever.recieve();
    }
    reciever.shutdown();
    return 0;
}