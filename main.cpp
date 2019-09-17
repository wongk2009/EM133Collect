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

void * EM133DataProcess(void *); 
void * TcpClientUploadProcess(void *); 

int main(int argc, char *argv[]){
    cout << "This is EM133 collector program." << endl;
//    CEM133Collector EM133Dev;
//    EM133Dev.SetUpTCPSocket("192.168.146.100", 502);
//    EM133Dev.SaveEM133Data();
//    cout << EM133Dev.m_Current_Time << endl;
//    EM133Dev.SaveEM133Data();
//    cout << EM133Dev.m_Current_Time << endl;
    pthread_t thread[2];
    pthread_mutex_t mut;
    pthread_mutex_init(&mut, NULL);
    pthread_create(&thread[0], NULL, EM133DataProcess, NULL);
    pthread_create(&thread[1], NULL, TcpClientUploadProcess, NULL);
    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    return 0;
}

void * EM133DataProcess(void *) {
    CEM133Collector EM133Dev;
    //EM133Dev.SetUpTCPSocket("169.254.142.221", 502);
    EM133Dev.SetUpTCPSocket("192.168.146.100", 502);
    //EM133Dev.SetUpFastTCPSocket("192.168.146.100", 502);
    while(1) {
        EM133Dev.UpdateLogFile();
        //EM133Dev.FastUpdateLogFile();
        //sleep(1);
    }
    pthread_exit(NULL);      
}

void * TcpClientUploadProcess(void *) {
    CtcpClient TcpClientDev;
    //TcpClientDev.CreateNewTcpSocket("192.168.146.100", 1502);
    while(1) {
        TcpClientDev.Upload_File();
        sleep(1);
}
    pthread_exit(NULL);      
}
