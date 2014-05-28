#ifndef CONNECTION_H
#define CONNECTION_H
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <QString>
#include <netdb.h>

class connection
{
public:
    connection();
    int newTry(char *, QString);
    void breakConnection(int, QString);
    QString sendNewMsg(char *, int);
    void sendNewChatMsg(char *, int);
    QString getNewChatMsg(int);
private:
    int sockfd;
    struct sockaddr_in servaddr;
    struct hostent *tmp;
};

#endif // CONNECTION_H
