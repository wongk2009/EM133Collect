#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include "CEM133Collector.h"
#include "CtcpClient.h"

using namespace std;

int Connect_Status = 0;
void * EM133DataProcess(void *); 
void * TcpClientUploadProcess(void *); 

int main(int argc, char *argv[]){
    cout << "This is EM133 collector program." << endl;

    //测试代码-1
//    CEM133Collector EM133Dev;
//    EM133Dev.SetUpTCPSocket("192.168.20.102", 502);
//    EM133Dev.QuickUpdateLogFile(); 
//    cout << EM133Dev.m_Current_Time << endl;
//    EM133Dev.QuickUpdateLogFile(); 
//    cout << EM133Dev.m_Current_Time << endl;

    //测试代码-2
//    EM133Dev.SaveEM133Data();
//    cout << EM133Dev.m_Current_Time << endl;
//    EM133Dev.SaveEM133Data();
//    cout << EM133Dev.m_Current_Time << endl;

    pthread_t thread[2];
    pthread_mutex_t mut;
    pthread_mutex_init(&mut, NULL);
    pthread_create(&thread[0], NULL, TcpClientUploadProcess, NULL);
    pthread_create(&thread[1], NULL, EM133DataProcess, NULL);
    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    return 0;
}

void * EM133DataProcess(void *) {
    CEM133Collector EM133Dev;
    EM133Dev.SetUpTCPSocket("192.168.146.100", 502);
    //EM133Dev.SetUpFastTCPSocket("192.168.146.100", 502);
    while(1) {
//	if(Connect_Status != 1) {
//            //EM133Dev.UpdateLogFile();
//            //EM133Dev.FastUpdateLogFile();
//            EM133Dev.QuickUpdateLogFile(); 
//            //sleep(1);
//	}
//	else {
//            //等待；
//	}
        EM133Dev.QuickUpdateLogFile(); 
	//sleep(1);
    }
    pthread_exit(NULL);      
}

void * TcpClientUploadProcess(void *) {
    CtcpClient TcpClientDev;
    TcpClientDev.CreateNewTcpSocket("122.51.1.204", 1502);
    while(1) {
        Connect_Status = TcpClientDev.Upload_File();
		TcpClientDev.CreateNewTcpSocket("122.51.1.204", 1502);
//		if(Connect_Status == 1) {
//			TcpClientDev.CreateNewTcpSocket("122.51.1.204", 1502);
//		}
        //sleep(1);
    }
    pthread_exit(NULL);      
}
