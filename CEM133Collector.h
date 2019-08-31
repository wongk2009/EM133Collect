#ifndef CEM133COLLECTOR_H
#define CEM133COLLECTOR_H

#include <iostream>
#include <modbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class CEM133Collector{
    public:
        CEM133Collector();
        ~CEM133Collector();

        //modbus参数设置
        int m_DEVICE_ID = 1;
        uint32_t m_old_response_to_sec;
        uint32_t m_old_response_to_usec;
        vector<uint16_t> m_tab_reg;

        int SetUpTCPServer(const char *ip, int port);
};

#endif //CEM133COLLECTOR_H
