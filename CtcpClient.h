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
#include <unistd.h>
#include <fstream>
#include <errno.h>
#include <cstdio>

#define SERVER_IP "122.51.1.204"
#define SERVER_PORT 1502
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512 
#define FILE_MAX_CNT 1000

using namespace std;

class CtcpClient{
    public:
        CtcpClient();
        ~CtcpClient();

		struct timeval timeout = {3,0}; 

        int CreateNewTcpSocket(const char *ip, const int port);
        int Upload_File();

    private:
        int sockfd;
        struct sockaddr_in servaddr; 

        unsigned int m_File_Cnt = 1;
        string m_File_Name; 
        string str_File_Name_Prefix = "Data/common.";
        string str_File_Name_Suffix = ".csv";
        string m_Current_File_Name;

        unsigned int m_Sent_Size = 0;
        unsigned int m_Full_Size = 0;
        unsigned int m_Remained_Size = 0;
        char buffer[BUFFER_SIZE];

        string Get_Current_File_Name();
        unsigned long Get_File_Size(const char *);
};


#endif
