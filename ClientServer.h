
#include <sys/socket.h> //socket, close?
#include <sys/types.h> //socket
// #include <winsock.h> //windows socket programing
#include <netdb.h> //addrinfo & getaddrinfo
#include <unistd.h> //fork & close?
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> //inet_ntop(convert ip addresses)
#include <netinet/in.h>
#include <sys/wait.h> //sa_handler
#include <signal.h> //sigaction
#include <string> //memset
#include <iostream> //cout
#define BACKLOG 10

using namespace std;

#define PORT "3490" // the port client will be connecting to 
#define MAXDATASIZE 100 // max number of bytes we can get at once

class Server {
private:
    struct addrinfo hints;      //system information
    int sockID;
    struct sockaddr_storage connector;
    socklen_t s_in_size;
    char servName[INET6_ADDRSTRLEN];
    char message[100];

public:
    Server();
    ~Server();

    void setMessage(char mess[], int length);
    char* getMessage() {return message;};
    
    bool setup();
    bool sendMessage();
    void shutdown();
    // void Wshutdown();

    void printHostname(); 
    void formatIP(sockaddr_storage & network, char s[], int s_size);

};

class Client {
private:
    struct addrinfo hints;           //system information
    // struct addrinfo* serverInfo;
    const char* hostname;            //name of the server
    int sockID;                      //ID for socket
    struct addrinfo* sock;           //socket inforamtion
    int numbytes;                    //number ob bytes recieved
    char message[100];                   //message received
    char servName[INET6_ADDRSTRLEN]; //name of the server
public: 
    Client();
    Client(const char* host);
    ~Client();

    bool setup();
    void recieve();
    // char* getMessage();
    void shutdown();
    // void Wshutdown();

    void formatIP(addrinfo*& network, char s[], int s_size);
};

int winSetup();

