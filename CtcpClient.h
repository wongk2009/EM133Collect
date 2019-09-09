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
#include <fstream>

using namespace std;

class CtcpClient{
    public:
        CtcpClient();
        ~CtcpClient();

        int CreateNewTcpSocket(const char *ip, const int port);
        
        unsigned long get_file_size(const char *);
        string Get_Current_File_Name();

    private:
        int sockfd;
        struct sockaddr_in servaddr; 

        unsigned int m_File_Cnt = 1;
        string m_File_Name; 
        string str_File_Name_Prefix = "Data/common.";
        string str_File_Name_Suffix = ".csv";
        string m_Current_File_Name;

        //string Get_Current_File_Name();
};


#endif
