#include "ClientServer.h"

Server::Server() {

}

Server::~Server() {

}

void Server::setMessage(char mess[], int length) {
    if(length > 100) {
        cout << "Invalid message" << endl;
        return;
    }
    for(int i = 0; i < length; i++) {
        message[i] = mess[i];
    }
    for(int i = length; i < 100; i++) {
        message[i] = '\0';
    }
}

bool Server::setup() {
    struct addrinfo* serverInfo;
    struct sigaction sa;

    memset(&hints, 0, sizeof hints); //clear memory in hints

    hints.ai_family = AF_UNSPEC;        //either IP4 or IP6
    hints.ai_socktype = SOCK_STREAM;    //TCP
    hints.ai_flags = AI_PASSIVE;        //use my IP

//getraddinfo
    int rv;
    if((rv = getaddrinfo(NULL,PORT,&hints,&serverInfo)) != 0) { //getaddrinfo fail
        cout << "error: getaddrinfo - " << rv << endl;
        return false; //exit: failed
    }
//bind to a socket
    sockID = socket(serverInfo->ai_family,serverInfo->ai_socktype,serverInfo->ai_protocol);
    if(sockID == -1) {
        cout << "server: socket" << endl;
        return false; //exit: failed
    }
    //reuse address
    int yes = 1;
    if(setsockopt(sockID, SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        cout << "error: setscokopt" << endl;
    }
    //bind
    if(bind(sockID,serverInfo->ai_addr,serverInfo->ai_addrlen) == -1) {
        cout << "server: bind" << endl;
        return false;
    }
//free memory
    freeaddrinfo(serverInfo);
//listen
    if(listen(sockID,BACKLOG) == -1) {
        cout << "error: listen" << endl;
        return false;
    }
//reap dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        cout << "error: sigaction" << endl;
        return false;
    }
//succeeded in connecting
    cout << "waiting for connection" << endl;
    return true;
}
bool Server::sendMessage() {
    socklen_t s_in_size = sizeof connector;
    int conID = accept(sockID, (struct sockaddr*)&connector, &s_in_size);
    if(conID == -1) {
        perror("accept");
        return false;
    }
    else {
        formatIP(connector,servName,sizeof servName);
        cout << "server: got connection from " << servName << endl;
    }
    if(!fork()) {
        close(sockID);
        if(send(conID,message,13,0) == -1) {
            perror("send");
            return false;
        }
        close(conID);
    }
    close(conID);
    return true;
}
void Server::printHostname() {
    char hostname[100];
    gethostname(hostname,100);
    for(int i = 0; i < 100; i++) {
        cout << hostname[i];
    }
}
void Server::shutdown() {
    close(sockID);
}
// void Server::Wshutdown() {
//     closesocket(sockID);
// }
//format and store the IP address
void Server::formatIP(sockaddr_storage & network, char s[], int s_size) {
    if(network.ss_family == AF_INET) { //IP5
        // struct sockaddr_in* sock_in = (struct sockaddr_in*)&network;
        inet_ntop(network.ss_family,(struct sockaddr_in*)&network, s, s_size);
    }
    else {// struct sockaddr_in6* sock_in = (struct sockaddr_in*)&network;
    inet_ntop(network.ss_family,((struct sockaddr_in6*)&network),s,s_size);
    }
}

// int winSetup() {
//     WSADATA wsaData;   // if this doesn't work
//     //WSAData wsaData; // then try this instead

//     // MAKEWORD(1,1) for Winsock 1.1, MAKEWORD(2,0) for Winsock 2.0:

//     if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
//         fprintf(stderr, "WSAStartup failed.\n");
//         return 1;
//     }
//     return 0;
// }