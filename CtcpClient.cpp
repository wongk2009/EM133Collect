#include "CtcpClient.h"

CtcpClient::CtcpClient(){
//    cout << Get_Current_File_Name() << endl;
}

CtcpClient::~CtcpClient(){
    close(sockfd);
}

/***************************************************************************************
*函数：CreateNewTcpSocket
*描述：创建tcp socket；
*参数：
*   ip：tcp连接的ip地址，默认ip为127.0.0.1；
*   port：tcp连接的端口，默认端口为502；
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CtcpClient::CreateNewTcpSocket(const char *ip = "127.0.0.1", const int port = 502){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); 
    inet_pton(AF_INET, ip, &servaddr.sin_addr); 

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout,sizeof(timeout));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout,sizeof(timeout));

    return 0;
}

/***************************************************************************************
*函数：Get_File_Size
*描述：获取文件大小；
*参数：
*   path：读取文件的路径；
*返回值：成功返回文件的大小，否则返回-1。
***************************************************************************************/
unsigned long CtcpClient::Get_File_Size(const char *path){
    unsigned long filesize = -1;	
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
    	return filesize;
    }
    else{
    	filesize = statbuff.st_size;
    }
    return filesize;
}

/***************************************************************************************
*函数：Get_Current_File_Name
*描述：获取当前待上传文件的文件名
*参数：
*   无
*返回值：返回文件的大小。
***************************************************************************************/
string CtcpClient::Get_Current_File_Name() {
    string strEmptyFile;
    do {
	    //是否已经重头开始保存文件
		if(m_File_Cnt == FILE_MAX_CNT) {
            ifstream in_RestartFg;
		    in_RestartFg.open("Data/common.1.csv");
		    if(in_RestartFg) {    
				m_File_Name = str_File_Name_Prefix + to_string(m_File_Cnt) + str_File_Name_Suffix;
			    m_Current_File_Name = m_File_Name;
			    m_File_Cnt = 1;
		        return m_Current_File_Name;
		    }
		}
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
                return strEmptyFile;
            }
        }
        else {
            //return strEmptyFile;
        }
    } while(1);
}

/***************************************************************************************
*函数：Upload_File
*描述：上传数据文件；
*参数：
*   无
*返回值：0：文件发送成功;
*        1：心跳连接失败；
*        2：发送文件不存在；
*        3：发送文件名失败；
*        4：发送文件大小失败；
*        5：文件发送错误；
*        6：删除已发送文件失败，
*        7：socket连接失败。
***************************************************************************************/
int CtcpClient::Upload_File() {
    int error_code = 0;
	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == 0){
			printf("Connect %s successfully!\n", SERVER_IP);
		}
		else{
			perror("connect");
			close(sockfd);
			return 1;
		}


	//发送心跳包
	memset(buffer, 0, BUFFER_SIZE);
    buffer[0] = 0x55;
	if (send(sockfd, buffer, BUFFER_SIZE, 0) < 0)
    {
            #ifndef NDEBUG
    		    printf("Server Connect Send Failed!\n");
    		#endif
            close(sockfd);
            return 1;
    }

	memset(buffer, 0, BUFFER_SIZE);
	
	if (recv(sockfd, buffer, BUFFER_SIZE, 0) < 0)
	{
		    #ifndef NDEBUG
			    printf("Server Connect Receive Failed!\n");
		    #endif 
			close(sockfd);
		    return 1;
	}
	else {
		if(buffer[0] != 0x55) {
			#ifndef NDEBUG
			    printf("Server Connect Failed!\n");
		    #endif
			close(sockfd);
		    return 1;
		}
	}

	//获取文件名
    string file_name = Get_Current_File_Name(); 
	if(file_name.empty()){
		#ifndef NDEBUG
			printf("File Not Found!\n");
		#endif
		//close(sockfd);
		return 2;
	}
    memset(buffer, 0, BUFFER_SIZE);

    FILE* fp = fopen(file_name.c_str(), "rb"); //windows下是"rb",表示打开一个只读的二进制文件 
    if (NULL == fp)
    {
            #ifndef NDEBUG
    	        printf("File: %s Not Found\n", file_name.c_str());
    	    #endif
            //close(sockfd);
            return 2;
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
                #ifndef NDEBUG
    		        printf("Send File Name Failed\n");
    		    #endif
                close(sockfd);
                return 3;
        }
        memset(buffer, 0, BUFFER_SIZE);

        m_Full_Size = Get_File_Size(file_name.c_str());
        string strFileSize = to_string(m_Full_Size);
        strncpy(buffer, strFileSize.c_str(), strlen(strFileSize.c_str()) > BUFFER_SIZE ? BUFFER_SIZE : strlen(strFileSize.c_str()));
        //向服务器发送文件大小 
        if (send(sockfd, buffer, BUFFER_SIZE, 0) < 0)
        {
                #ifndef NDEBUG
                    printf("Send File Size Failed\n");
                #endif
                close(sockfd);
                return 4;
        }
        memset(buffer, 0, BUFFER_SIZE);

    	int length = 0;
    	while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
    	{
               m_Sent_Size += length;
               m_Remained_Size = m_Full_Size - m_Sent_Size; 
			   if (send(sockfd, buffer, length, 0) < 0) {
		            #ifndef NDEBUG
	    		        printf("Send File: %s Failed\n", file_name.c_str());
			            perror("Send Error:");
			        #endif
					close(sockfd);
		            return 5;
	    		    //break;
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
		    #ifndef NDEBUG
                    cout << "Remove File Failed!" << endl;
		    #endif
	        error_code = 6;
        }
    }
    //close(sockfd);
    return error_code;
}
