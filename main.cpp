#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "CEM133Collector.h"
#include "CtcpClient.h"
#include <stdio.h>

#define BUFFER_SIZE 1024 
#define FILE_NAME_MAX_SIZE 512 

using namespace std;

int main(int argc, char *argv[]){
    cout << "This is EM133 collector program." << endl;
//    CEM133Collector EM133Dev;
//    EM133Dev.SetUpTCPServer("192.168.146.100", 502);
    CtcpClient TcpClientDev;
    TcpClientDev.CreateNewTcpSocket("192.168.146.100", 502);
    while(1) {
       // EM133Dev.Update_Log_File();
        cout << TcpClientDev.Get_Current_File_Name() << endl;
        sleep(1);
    }

//    long long filesize = TcpClientDev.get_file_size("2019-08-25-131622.jpg");
//    cout << "FileSize: " << filesize << endl;
//
//    //输入文件名 
//    char file_name[FILE_NAME_MAX_SIZE + 1];
//    memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
//    printf("Please Input File Name On Server: ");
//    scanf("%s", &file_name);
//
//    char buffer[BUFFER_SIZE];
//    memset(buffer, 0, BUFFER_SIZE);
//
//    FILE* fp = fopen(file_name, "rb"); //windows下是"rb",表示打开一个只读的二进制文件 
//    if (NULL == fp)
//    {
//	printf("File: %s Not Found\n", file_name);
//    }
//    else
//    {
//        strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
//
//        //向服务器发送文件名 
//        if (send(TcpClientDev.sockfd, buffer, BUFFER_SIZE, 0) < 0)
//        {
//        	printf("Send File Name Failed\n");
//         	system("pause");
//	        exit(1);
//        }
//	memset(buffer, 0, BUFFER_SIZE);
//	int length = 0;
//
//	while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
//	{
//          	if (send(TcpClientDev.sockfd, buffer, length, 0) < 0)
//		{
//			printf("Send File: %s Failed\n", file_name);
//			break;
//		}
//		memset(buffer, 0, BUFFER_SIZE);
//	}
//
//	fclose(fp);
//	printf("File: %s Transfer Successful!\n", file_name);
//    }
    return 0;
}
