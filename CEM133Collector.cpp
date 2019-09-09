#include "CEM133Collector.h"

CEM133Collector::CEM133Collector(){
    //设置modbus超时时间为1000毫秒
    m_old_response_to_sec = 0;
    m_old_response_to_usec = 1000000;
    
    sys_Usec_Time();
    system("mkdir Data/");
    Create_New_Log_File();
}

CEM133Collector::~CEM133Collector(){

}

int CEM133Collector::SetUpTCPServer(const char *ip = "127.0.0.1", int port = 1502){
    ctx = modbus_new_tcp(ip, port);
    modbus_set_debug(ctx, TRUE);

    if (modbus_connect(ctx) == -1) {
	fprintf(stderr, "Connection failed: %s\n",
	modbus_strerror(errno));
	modbus_free(ctx);
	return -1;
    }
    //设置从机地址
    int rc = modbus_set_slave(ctx, m_DEVICE_ID);
    if (rc == -1) {
        fprintf(stderr, "Invalid slave ID\n");
        modbus_free(ctx);
        return -1;
    }
    //建立modbus连接
    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
    modbus_get_response_timeout(ctx, &m_old_response_to_sec, &m_old_response_to_usec);
    return 0;
}

int CEM133Collector::ReadEM133Data(modbus_t *ctx, int addr, int nb, uint16_t *dest) {

    int rc = modbus_read_registers(ctx, addr, nb, dest);
    if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }

    for (int i = 0; i < rc; i++) {
         printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
    }
    return rc;
}

int CEM133Collector::CloseTCPServer(modbus_t *ctx) {
    modbus_close(ctx);
    modbus_free(ctx);
    return 0;
}

int CEM133Collector::SaveEM133Data() {
    int rc1 = ReadEM133Data(ctx, 13312, 30, tab_reg);
    m_tab_reg1.clear();
    for (int i = 0; i < rc1; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg1.push_back(lTmpData);
         cout << "m_tab_reg1[" << m_tab_reg1.size() - 1 << "]= " << m_tab_reg1.back() << endl;
    }
    int rc2 = ReadEM133Data(ctx, 13696, 20, tab_reg);
    m_tab_reg2.clear();
    for (int i = 0; i < rc2; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg2.push_back(lTmpData);
         cout << "m_tab_reg2[" << m_tab_reg2.size() - 1 << "]= " << m_tab_reg2.back() << endl;
    }
    int rc3 = ReadEM133Data(ctx, 13824, 10, tab_reg);
    m_tab_reg3.clear();
    for (int i = 0; i < rc3; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg3.push_back(lTmpData);
         cout << "m_tab_reg3[" << m_tab_reg3.size() - 1 << "]= " << m_tab_reg3.back() << endl;
    }

    sys_Usec_Time();
    cout << m_Current_Time << endl;
    
    return ((rc1 = -1) || (rc2 = -1) || (rc3 = -1)) ? -1 : 1;
}
 
string CEM133Collector::sys_Usec_Time()
{
    struct timeval tv;
    struct timezone tz;
    struct tm *p; 

    gettimeofday(&tv, &tz);
/*
    printf("tv_sec:%ld\n",tv.tv_sec);
    printf("tv_usec:%ld\n",tv.tv_usec);
    printf("tz_minuteswest:%d\n",tz.tz_minuteswest);
    printf("tz_dsttime:%d\n",tz.tz_dsttime);
*/    
    p = localtime(&tv.tv_sec);
    //printf("time_now:%d %d %d %d %d %d.%ld\n", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec, tv.tv_usec);
    
    string strYear = to_string(1900+p->tm_year);
    string strMonth = to_string(1+p->tm_mon);
    string strDay = to_string(p->tm_mday);
    string strHour = to_string(p->tm_hour);
    string strMin = to_string(p->tm_min);
    string strSec = to_string(p->tm_sec);
    string strUsec = to_string(tv.tv_usec);

    m_Current_Time = strYear + "-" + strMonth + "-" + strDay + " " + strHour + ":" + strMin + ":" + strSec + "." + strUsec;
    return m_Current_Time;
}

int CEM133Collector::Create_New_Log_File() {
    m_File_Name = str_File_Name_Prefix + to_string(m_File_Cnt) + str_File_Name_Suffix;
    ofstream out;
    out.open(m_File_Name, ofstream::out);
    if(out) {
         out << "Date,V1,V2,V3,I1,I2,I3,Total kW,Total kvar,Total kVA,Total PF,Frequency" << endl;	
    } 
    else {
         return -1;
    }  
    out.close();
    return 0;
}

int CEM133Collector::Update_Log_File() {
    if(m_Data_Cnt == MAX_DATA_CNT) {
         m_Data_Cnt = 0;
         m_File_Cnt++;
         if(m_File_Cnt == 50000) {
             m_File_Cnt = 1;
         }
         Create_New_Log_File();
    }
    m_Data_Cnt++;
    if(CEM133Collector::SaveEM133Data()) {
         ofstream out;
         out.open(m_File_Name, ofstream::out | ofstream::app);
         if(out) {
              out << m_Current_Time << "," << m_tab_reg1[0] << ","  << m_tab_reg1[1] << ","  << m_tab_reg1[2] << ","  
                  << m_tab_reg1[3] << ","  << m_tab_reg1[4] << ","  << m_tab_reg1[5] << ","  
                  << m_tab_reg2[0] << ","  << m_tab_reg2[1] << ","  << m_tab_reg2[2] << ","  << m_tab_reg2[3] << "," 
                  << m_tab_reg3[2] << endl;
         } 
         else {
              return -1;
         }  
         out.close();
    }
    else {
         return -1;
    }
    return 0;
}
