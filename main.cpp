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
    EM133Dev.SetUpTCPServer("192.168.146.100", 502);
    while(1) {
        EM133Dev.Update_Log_File();
        //printf("----------EM133Dev Test---------\n");
        //sleep(1);
}
    pthread_exit(NULL);      
}

void * TcpClientUploadProcess(void *) {
    CtcpClient TcpClientDev;
    //TcpClientDev.CreateNewTcpSocket("192.168.146.100", 1502);
    while(1) {
        TcpClientDev.Upload_File();
        //printf("TCP Test.\n");
        sleep(1);
}
    pthread_exit(NULL);      
}
