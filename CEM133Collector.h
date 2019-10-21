#ifndef CEM133COLLECTOR_H
#define CEM133COLLECTOR_H

#include <iostream>
#include <modbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <fstream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


using namespace std;

#define NDEBUG
#define MAX_DATA_CNT 10000
#define MAX_FILE_CNT 1000
#define BUFFER_SIZE 1024 

class CEM133Collector{
    public:
        CEM133Collector();
        ~CEM133Collector();

        //modbus参数设置
        uint16_t m_DEVICE_ID = 1;
        uint32_t m_old_response_to_sec;
        uint32_t m_old_response_to_msec;       
        uint32_t m_old_response_to_usec;       

        uint16_t tab_reg[1024];
        string m_Current_Time;

        int SetUpTCPSocket(const char *ip, int port);
		int CloseTCPSocket();
        int ReadEM133Data(modbus_t *ctx, int addr, int nb, uint16_t *dest);
        int CloseTCPServer(modbus_t *ctx);
        int SaveEM133Data();
        int UpdateLogFile();
        int FastUpdateLogFile();

        char buffer[BUFFER_SIZE];
        int SetUpFastTCPSocket(const char *ip, int port);
        int FastReadEM133Data(int sockfd, int addr, int nb, uint16_t *dest);
        int FastSaveEM133Data();

        int ReadEM133SingleCMD();
        int QuickUpdateLogFile(); 

private:
        modbus_t *ctx;
        int sockfd;
        struct sockaddr_in servaddr; 
        uint32_t m_Transaction_ID = 0;
        char read_registers_cmd[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x03, 0x34, 0x00, 0x00, 0x1E};

        unsigned int m_File_Cnt = 1;
        unsigned int m_Data_Cnt = 0;

        string m_File_Name; 
        string str_File_Name_Prefix = "Data/common.";
        string str_File_Name_Suffix = ".csv";

        time_t curtime;
        //string m_Current_Time;
        vector<long> m_tab_reg1;
        vector<long> m_tab_reg2;
        vector<long> m_tab_reg3;

        string sys_Usec_Time();
        int Create_New_Log_File();

};

#endif //CEM133COLLECTOR_H
