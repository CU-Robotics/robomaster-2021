#include "ClientServer.h"

int main()
{
    // Client reciever ("Dantes-MacBook-Pro.local");
    Server sender;
    if(!sender.setup()) return 1;
    char* sendThis = "O time thy pyramids";
    sender.setMessage(sendThis,20);
    if(!sender.sendMessage()) return 1;
    sender.shutdown();
}