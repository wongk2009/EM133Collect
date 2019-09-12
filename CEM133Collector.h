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

using namespace std;

#define MAX_DATA_CNT 100
#define MAX_FILE_CNT 100

class CEM133Collector{
    public:
        CEM133Collector();
        ~CEM133Collector();

        //modbus参数设置
        int m_DEVICE_ID = 1;
        uint32_t m_old_response_to_sec;
        uint32_t m_old_response_to_usec;       

        modbus_t *ctx;
        uint16_t tab_reg[1024];

        int SetUpTCPServer(const char *ip, int port);
        int ReadEM133Data(modbus_t *ctx, int addr, int nb, uint16_t *dest);
        int CloseTCPServer(modbus_t *ctx);
        int SaveEM133Data();
        int Update_Log_File();

    private:
        unsigned int m_File_Cnt = 1;
        unsigned int m_Data_Cnt = 0;

        string m_File_Name; 
        string str_File_Name_Prefix = "Data/common.";
        string str_File_Name_Suffix = ".csv";

        time_t curtime;
        vector<long> m_tab_reg1;
        vector<long> m_tab_reg2;
        vector<long> m_tab_reg3;

        string sys_Usec_Time();
        string m_Current_Time;
        int Create_New_Log_File();

};

#endif //CEM133COLLECTOR_H
