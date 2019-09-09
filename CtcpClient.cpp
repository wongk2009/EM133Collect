#include "CtcpClient.h"

CtcpClient::CtcpClient(){
//    cout << Get_Current_File_Name() << endl;
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

string CtcpClient::Get_Current_File_Name() {
    do {
        m_File_Name = str_File_Name_Prefix + to_string(m_File_Cnt) + str_File_Name_Suffix;
        ifstream in_init;
        in_init.open(m_File_Name);
        if(in_init) {
            string tmp_File_Name = str_File_Name_Prefix + to_string(m_File_Cnt + 1) + str_File_Name_Suffix;
            ifstream in_next;
            in_next.open(tmp_File_Name);
            if(in_next) {
                m_File_Cnt++;
                m_Current_File_Name = m_File_Name;
                return m_Current_File_Name;
            }
            else {
                return string("error");
            }
        }
        else {
            return string("error");
        }
    } while(1);
}
