#include "ClientServer.h"

Client::Client(const char* host) {
    hostname = host;
}
Client::~Client() { }

//link the client with the host
bool Client::setup() {
    struct addrinfo* serverInfo; //different socket informations

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
//getaddrinfo
    int rv; //return value
    if((rv = getaddrinfo(hostname, PORT, &hints, &serverInfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }
//try to connect
    struct addrinfo* crawler;
    for(crawler = serverInfo; crawler != NULL; crawler = crawler->ai_next) {
    //socket
        if((sockID = socket(crawler->ai_family, crawler->ai_socktype,crawler->ai_protocol)) == -1) {
            perror("clent: socket");
            continue;
        }
        int yes = 1;
        if(setsockopt(sockID, SOL_SOCKET,SO_REUSEADDR, &yes,sizeof(int)) == -1) {
            perror("error: setsockopt");
            continue;
        }
    //bind?
        // if(bind(sockID,crawler->ai_addr,crawler->ai_addrlen) == -1) {
        //     perror("client: bind");
        //     continue;
        // }
    //connect
        if(connect(sockID, crawler->ai_addr, crawler->ai_addrlen) == -1) {
            close(sockID);
            perror("client: connect");
            continue;
        }
        break;
    }
//check if connection successful
    if(crawler == NULL) { //no good sockets
        cout << stderr << "client: failed to connect" << endl;
        return false;
    }
//update socket
    sock = crawler;
//print connection
    formatIP(sock,servName,46);
    cout << "client: connecting to:" << servName << endl;
//free linked list with server info
    freeaddrinfo(serverInfo);
//Success, quit
    return true;
}
//recieve a message
void Client::recieve() {
    if ((numbytes = recv(sockID, message, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return;
    }
    if(numbytes > 0) {
        message[numbytes] = '\0';
        cout << "client: received: " << message << endl;

        return;
    }
    return;
}
//close the socket and free any memory
void Client::shutdown() {
    close(sockID);
}
// void Client::Wshutdown() { //shutdown but windows
//     closesock(sockID);
// }
//format and store the IP address
void Client::formatIP(addrinfo*& network, char s[], int s_size) {
    if(network->ai_family == AF_INET) { //IP5
        // struct sockaddr_in* sock_in = (struct sockaddr_in*)&network;
        inet_ntop(network->ai_family,(struct sockaddr_in*)&network, s, s_size);
    }
    else {// struct sockaddr_in6* sock_in = (struct sockaddr_in*)&network;
    inet_ntop(network->ai_family,((struct sockaddr_in6*)&network),s,s_size);
    }
}
