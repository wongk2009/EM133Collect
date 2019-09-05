#ifndef CTCPCLIENT_H
#define CTCPCLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>

using namespace std;

class CtcpClient{
    public:
        CtcpClient();
        ~CtcpClient();

        int CreateNewTcpSocket(const char *ip, const int port);
        
        unsigned long get_file_size(const char *);

//    private:
        int sockfd;
        struct sockaddr_in servaddr; 
};


#endif
