#include "CtcpClient.h"

CtcpClient::CtcpClient(){

}

CtcpClient::~CtcpClient(){

}

int CtcpClient::CreateNewTcpSocket(const char *ip, const int port){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); 
    inet_pton(AF_INET, ip, &servaddr.sin_addr); 
    if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == 0){
        printf("Connect %s successfully!\n", ip);
    }
    else{
        perror("connect");
	return -1;
    }
    return 0;
}
unsigned long CtcpClient::get_file_size(const char *path){
    unsigned long filesize = -1;	
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
	return filesize;
    }else{
	filesize = statbuff.st_size;
    }
    return filesize;
}
