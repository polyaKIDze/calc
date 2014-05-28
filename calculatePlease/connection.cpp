#include "connection.h"
#include <iostream>

connection::connection(){}

int connection::newTry(char *address, QString nickName)
{

    sockfd = socket (PF_INET, SOCK_STREAM, 0); //QAbstractSocket::TcpSocket
    if (sockfd < 0)
    {
        perror (NULL);
        return -1;
    }

    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons (51000);

    tmp = gethostbyname(address);
    if (!tmp) {
            printf("Lookup Failed: %s\n", hstrerror(h_errno));
            return 0;
    }
    if (inet_aton(inet_ntoa( (struct in_addr) *((struct in_addr *) tmp->h_addr_list[0])), &servaddr.sin_addr) == 0)
  //  if (inet_aton(address, &servaddr.sin_addr) == 0)
    {
        printf ("Invalid IP address\n");
        close (sockfd);
        exit (1);
    }

    if (connect (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr)) < 0)
    {
        perror (NULL);
        close (sockfd);
        return -1;
    }
    int n;
    char startMsg[50];
    strcpy(startMsg, "newbie:");
    strcat(startMsg, nickName.toAscii().data());
    n = write(sockfd, startMsg, strlen(startMsg) + 1);
    if (n < 0)
    {
        perror ("NULL");
        close (sockfd);
    }
    return sockfd;
}

void connection::breakConnection(int sockfd, QString nickName)
{
    int n;
    char stopMsg[50];
    strcpy(stopMsg, "exit:");
    strcat(stopMsg, nickName.toAscii().data());
    n = write(sockfd, stopMsg, strlen(stopMsg) + 1);
    if (n < 0)
    {
        perror ("NULL");
        close (sockfd);
    }
}

QString connection::sendNewMsg(char *sendline, int sockfd)
{
 /*   FILE *f;
    int n;
    char recvline[1000];
    bzero (recvline, 1000);
    if ((f = fopen ("logClient.txt", "a")) < 0) {
        perror (NULL);
    }
    fprintf (f, "Задание получено от пользователя: ");
    fprintf (f, "%s", sendline);

    n = write(sockfd, sendline, strlen(sendline) + 1);
    if (n < 0) {
        perror ("NULL");
        close (sockfd);
    }

    fprintf (f, "\nЗадание отправлено на сервер.\n");

    n = read (sockfd, recvline, 999);
    if (n < 0) {
        perror ("NULL");
        close (sockfd);
    }

    fprintf (f, "Ответ получен: %s\n\n", recvline);
    fclose (f);
    QString recv = QString(recvline);
    return recv;*/
}

void connection::sendNewChatMsg(char *sendline, int sockfd)
{
    int n;
    n = write(sockfd, sendline, strlen(sendline) + 1);
    if (n < 0) {
        perror ("NULL");
       close (sockfd);
    }
}

QString connection::getNewChatMsg(int sockfd)
{
    int n;
    char recvline[1000];
    bzero (recvline, 1000);
    n = read(sockfd, recvline, 999);
    if (n < 0) {
        perror ("NULL");
        close (sockfd);
    }
    QString recv = QString(recvline);
    return recv;
}
