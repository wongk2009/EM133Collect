#include "CtcpClient.h"

CtcpClient::CtcpClient(){
//    cout << Get_Current_File_Name() << endl;
}

CtcpClient::~CtcpClient(){
    close(sockfd);
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
                //return m_File_Name;
            }
        }
        else {
            //return m_File_Name;
        }
    } while(1);
}

int CtcpClient::Upload_File() {
    CreateNewTcpSocket("193.112.215.89", 1502);

    string file_name = Get_Current_File_Name(); 
    memset(buffer, 0, BUFFER_SIZE);

    FILE* fp = fopen(file_name.c_str(), "rb"); //windows下是"rb",表示打开一个只读的二进制文件 
    if (NULL == fp)
    {
	printf("File: %s Not Found\n", file_name.c_str());
        return -1;
    }
    else
    {
        string::iterator ItFileNameBeg = file_name.begin() + 5;
        string::iterator ItFileNameEnd = file_name.end();
        int iSubStrNum = ItFileNameEnd - ItFileNameBeg;
        string tmp_File_Name_Last = file_name.substr(5, iSubStrNum);
        strncpy(buffer, tmp_File_Name_Last.c_str(), strlen(tmp_File_Name_Last.c_str()) > BUFFER_SIZE ? BUFFER_SIZE : strlen(tmp_File_Name_Last.c_str()));

        //向服务器发送文件名 
        if (send(sockfd, buffer, BUFFER_SIZE, 0) < 0)
        {
        	printf("Send File Name Failed\n");
                return -1;
        }
	memset(buffer, 0, BUFFER_SIZE);

        m_Full_Size = get_file_size(file_name.c_str());
        string strFileSize = to_string(m_Full_Size);
        strncpy(buffer, strFileSize.c_str(), strlen(strFileSize.c_str()) > BUFFER_SIZE ? BUFFER_SIZE : strlen(strFileSize.c_str()));
        //向服务器发送文件大小 
        if (send(sockfd, buffer, BUFFER_SIZE, 0) < 0)
        {
        	printf("Send File Name Failed\n");
                return -1;
        }
	memset(buffer, 0, BUFFER_SIZE);

	int length = 0;
	while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
	{
                m_Sent_Size += length;
                m_Remained_Size = m_Full_Size - m_Sent_Size;
          	if (send(sockfd, buffer, length, 0) < 0)
		{
			printf("Send File: %s Failed\n", file_name.c_str());
			break;
		}
		memset(buffer, 0, BUFFER_SIZE);
	}

        m_Sent_Size = 0;
        m_Full_Size = 0;
        m_Remained_Size = 0;
	fclose(fp);
	printf("File: %s Transfer Successful!\n", file_name.c_str());
        if(remove(file_name.c_str()) < 0)
        {
           cout << "Remove File Failed!" << endl;        
        }
    }
    return 0;
}
