#include "CEM133Collector.h" 

CEM133Collector::CEM133Collector(){
    //设置modbus超时时间为1000毫秒
    m_old_response_to_sec = 0;
    m_old_response_to_msec = 1000;
    m_old_response_to_usec = 1000000;
    
    sys_Usec_Time();
    system("mkdir Data/");
	system("rm Data/*");
	Create_New_Log_File();
}

CEM133Collector::~CEM133Collector(){
    close(sockfd);
	modbus_close(ctx);
	modbus_free(ctx);
}

/***************************************************************************************
*函数：SetUpTCPSocket
*描述：建立EM133 socket连接（采用libmodbus第三方库）；
*参数：
*    ip：EM133的ip地址，默认为127.0.0.1；
*    port：EM133的端口地址，默认为1502；
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::SetUpTCPSocket(const char *ip = "127.0.0.1", int port = 1502){
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

/***************************************************************************************
*函数：CloseTCPSocket
*描述：关闭EM133 socket连接（采用libmodbus第三方库）；
*参数：
*    无
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::CloseTCPSocket() {
	modbus_close(ctx);
	modbus_free(ctx);
	int rc = close(sockfd);
    if(rc == -1) {
		return -1;
    }
	return 0;
}


/***************************************************************************************
*函数：SetUpTCPSocket
*描述：建立EM133 socket连接（采用libmodbus第三方库）；
*参数：
*    ip：EM133的ip地址，默认为127.0.0.1；
*    port：EM133的端口地址，默认为1502；
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::ReadEM133Data(modbus_t *ctx, int addr, int nb, uint16_t *dest) {
    int rc = modbus_read_registers(ctx, addr, nb, dest);
    if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }

    #ifndef NDEBUG
    for (int i = 0; i < rc; i++) {
         printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
    }
    #endif
    return rc;
}

/***************************************************************************************
*函数：CloseTCPServer
*描述：关闭EM133 socket；
*参数：
*    ctx：EM133的socket文件描述符；
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::CloseTCPServer(modbus_t *ctx) {
    modbus_close(ctx);
    modbus_free(ctx);
    return 0;
}

/***************************************************************************************
*函数：SaveEM133Data
*描述：保存EM133S数据；
*参数：
*    无
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::SaveEM133Data() {
    int rc1 = ReadEM133Data(ctx, 13312, 30, tab_reg);
    m_tab_reg1.clear();
    for (int i = 0; i < rc1; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg1.push_back(lTmpData);
         #ifndef NDEBUG
             cout << "m_tab_reg1[" << m_tab_reg1.size() - 1 << "]= " << m_tab_reg1.back() << endl;
         #endif
    }
    int rc2 = ReadEM133Data(ctx, 13696, 20, tab_reg);
    m_tab_reg2.clear();
    for (int i = 0; i < rc2; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg2.push_back(lTmpData);
         #ifndef NDEBUG
             cout << "m_tab_reg2[" << m_tab_reg2.size() - 1 << "]= " << m_tab_reg2.back() << endl;
         #endif
    }
    int rc3 = ReadEM133Data(ctx, 13824, 10, tab_reg);
    m_tab_reg3.clear();
    for (int i = 0; i < rc3; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg3.push_back(lTmpData);
         #ifndef NDEBUG
             cout << "m_tab_reg3[" << m_tab_reg3.size() - 1 << "]= " << m_tab_reg3.back() << endl;
         #endif
    }

    sys_Usec_Time();
    
    return ((rc1 == -1) || (rc2 == -1) || (rc3 == -1)) ? -1 : 0;
}
 
/***************************************************************************************
*函数：sys_Usec_Time
*描述：获取系统时间；
*参数：
*    无
*返回值：返回系统时间string；
***************************************************************************************/
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

/***************************************************************************************
*函数：Creat_New_Log_File
*描述：创建新的数据文件；
*参数：
*    无
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::Create_New_Log_File() {
    m_File_Name = str_File_Name_Prefix + to_string(m_File_Cnt) + str_File_Name_Suffix;
    ofstream out;
    out.open(m_File_Name, ofstream::out);
    if(out) {
         //out << "Date,V1,V2,V3,I1,I2,I3,Total kW,Total kvar,Total kVA,Total PF,Frequency" << "\r\n";	
         out << "Date,V1,V2,V3,I1,I2,I3,Total kW,Total kvar,Total kVA,Total PF,Frequency,kW L1,kW L2,kW L3,kvar L1,kvar L2,kvar L3,kVA L1,kVA L2,kVA L3,Power factor L1,Power factor L2,Power factor L3" << "\r\n";	
    } 
    else {
         return -1;
    }  
    out.close();
    return 0;
}

/***************************************************************************************
*函数：UpdateLogFile
*描述：更新数据文件，将新读取的数据写入数据文件中（采用libmodbus第三方库）；
*参数：
*    无
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::UpdateLogFile() {
    if(m_Data_Cnt == MAX_DATA_CNT) {
         m_Data_Cnt = 0;
         m_File_Cnt++;
         if(m_File_Cnt == MAX_FILE_CNT + 1) {
             m_File_Cnt = 1;
         }
         Create_New_Log_File();
    }
    m_Data_Cnt++;
	int rc = SaveEM133Data();
    if(rc == 0) {
         ofstream out;
         out.open(m_File_Name, ofstream::out | ofstream::app);
         if(out) {
              out << m_Current_Time << "," << m_tab_reg1[0] << ","  << m_tab_reg1[1] << ","  << m_tab_reg1[2] << ","  
                  << m_tab_reg1[3] << ","  << m_tab_reg1[4] << ","  << m_tab_reg1[5] << ","  
                  << m_tab_reg2[0] << ","  << m_tab_reg2[1] << ","  << m_tab_reg2[2] << ","  << m_tab_reg2[3] << "," 
                  << m_tab_reg3[2] << "\r\n";
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

/***************************************************************************************
*函数：SetUpFastTCPSocket
*描述：创建EM133 socket连接（采用Linux C库）；
*参数：
*    ip：EM133的ip地址，默认为127.0.0.1；
*    port：EM133的端口地址，默认为1502；
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::SetUpFastTCPSocket(const char *ip = "127.0.0.1", int port = 1502){
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); 
    inet_pton(AF_INET, ip, &servaddr.sin_addr); 
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&m_old_response_to_msec, sizeof(uint32_t));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&m_old_response_to_msec, sizeof(uint32_t));
    if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == 0){
        printf("Connect %s successfully!\n", ip);
    }
    else{
        perror("connect");
	return -1;
    }
    read_registers_cmd[6] = m_DEVICE_ID; 
    return 0;
}

/***************************************************************************************
*函数：FastReadEM133Data
*描述：读取EM133数据（使用Linux C库）；
*参数：
*    ctx：EM133的socket文件描述符；
*    addr：读取的地址；
*    nb：读取的数据数量；
*    dest：保存数据的数组首地址；
*返回值：成功返回读取数据的数量，否则返回-1。
***************************************************************************************/
int CEM133Collector::FastReadEM133Data(int sockfd, int addr, int nb, uint16_t *dest) {
    uint16_t Transaction_ID_Hi = (m_Transaction_ID >> 8) & 0xFF;
    uint16_t Transaction_ID_Lo = m_Transaction_ID & 0xFF;
    read_registers_cmd[0] = Transaction_ID_Hi;
    read_registers_cmd[1] = Transaction_ID_Lo;
    m_Transaction_ID++;
    uint16_t addr_Hi = (addr >> 8) & 0xFF;
    uint16_t addr_Lo = addr & 0xFF;
    read_registers_cmd[8] = addr_Hi;
    read_registers_cmd[9] = addr_Lo;
    uint16_t nb_Hi = (nb >> 8) & 0xFF;
    uint16_t nb_Lo = nb & 0xFF;
    read_registers_cmd[10] = nb_Hi;
    read_registers_cmd[11] = nb_Lo;

    memset(buffer, 0, BUFFER_SIZE);
    memcpy(buffer, read_registers_cmd, sizeof(read_registers_cmd) > BUFFER_SIZE ? BUFFER_SIZE : sizeof(read_registers_cmd));
    if (send(sockfd, buffer, sizeof(read_registers_cmd), 0) < 0)
    {
      	printf("Read Registers Failed\n");
        return -1;
    }

    memset(buffer, 0, BUFFER_SIZE);
    int rc = recv(sockfd, buffer, BUFFER_SIZE, 0);
    if (rc == -1) {
        perror("Receive Error");
        return -1;
    }
    memset(dest, 0, BUFFER_SIZE);
    //memcpy(dest, buffer, rc);

    int iDataCnt = buffer[8];
    for (int i = 0, j = 0; i < iDataCnt; i = i + 2, j++) {
	 uint16_t uiData_Hi = buffer[i + 9] & 0x00FF;
         uint16_t uiData_Lo = buffer[i + 9 + 1] & 0x00FF;
         *(dest + j) = (uiData_Hi << 8) + uiData_Lo;
         #ifndef NDEBUG
             printf("reg[%d]=%d (0x%X)\n", j, *(dest + j), *(dest + j));
         #endif
    }
    memset(buffer, 0, BUFFER_SIZE);
    return rc;
}

/***************************************************************************************
*函数：FastSaveEM133Data
*描述：保存EM133S数据（使用Linux C库）；
*参数：
*    无
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::FastSaveEM133Data() {
    int rc1 = FastReadEM133Data(sockfd, 13312, 30, tab_reg);
    m_tab_reg1.clear();
    for (int i = 0; i < rc1; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg1.push_back(lTmpData);
         #ifndef NDEBUG
             cout << "m_tab_reg1[" << m_tab_reg1.size() - 1 << "]= " << m_tab_reg1.back() << endl;
         #endif
    }
    int rc2 = FastReadEM133Data(sockfd, 13696, 20, tab_reg);
    m_tab_reg2.clear();
    for (int i = 0; i < rc2; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg2.push_back(lTmpData);
         #ifndef NDEBUG
             cout << "m_tab_reg2[" << m_tab_reg2.size() - 1 << "]= " << m_tab_reg2.back() << endl;
         #endif
    }
    int rc3 = FastReadEM133Data(sockfd, 13824, 10, tab_reg);
    m_tab_reg3.clear();
    for (int i = 0; i < rc3; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg3.push_back(lTmpData);
         #ifndef NDEBUG
             cout << "m_tab_reg3[" << m_tab_reg3.size() - 1 << "]= " << m_tab_reg3.back() << endl;
         #endif
    }

    sys_Usec_Time();
    
    return ((rc1 == -1) || (rc2 == -1) || (rc3 == -1)) ? -1 : 0;
}

/***************************************************************************************
*函数：FastUpdateLogFile
*描述：更新数据文件，将新读取的数据写入数据文件中（使用Linux C库）；
*参数：
*    无
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::FastUpdateLogFile() {
    if(m_Data_Cnt == MAX_DATA_CNT) {
         m_Data_Cnt = 0;
         m_File_Cnt++;
         if(m_File_Cnt == MAX_FILE_CNT + 1) {
             m_File_Cnt = 1;
         }
         Create_New_Log_File();
    }
    m_Data_Cnt++;
	int rc = FastSaveEM133Data();
    if(rc == 0) {
         ofstream out;
         out.open(m_File_Name, ofstream::out | ofstream::app);
         if(out) {
              out << m_Current_Time << "," << m_tab_reg1[0] << ","  << m_tab_reg1[1] << ","  << m_tab_reg1[2] << ","  
                  << m_tab_reg1[3] << ","  << m_tab_reg1[4] << ","  << m_tab_reg1[5] << ","  
                  << m_tab_reg2[0] << ","  << m_tab_reg2[1] << ","  << m_tab_reg2[2] << ","  << m_tab_reg2[3] << "," 
                  << m_tab_reg3[2] << "\r\n";
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

/***************************************************************************************
*函数：ReadEM133SingleCMD
*描述：读取EM133两条报文；
*参数：
*    无
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::ReadEM133SingleCMD() {
    int rc1 = ReadEM133Data(ctx, 13312, 36, tab_reg);
    m_tab_reg1.clear();
    for (int i = 0; i < rc1; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg1.push_back(lTmpData);
         #ifndef NDEBUG
             cout << "m_tab_reg1[" << m_tab_reg1.size() - 1 << "]= " << m_tab_reg1.back() << endl;
         #endif
    }

    int rc2 = ReadEM133Data(ctx, 13696, 20, tab_reg);
    m_tab_reg2.clear();
    for (int i = 0; i < rc1; i++) {
         long lTmpData = tab_reg[i] + (tab_reg[++i] << 16);
         m_tab_reg2.push_back(lTmpData);
         #ifndef NDEBUG
             cout << "m_tab_reg2[" << m_tab_reg2.size() - 1 << "]= " << m_tab_reg2.back() << endl;
         #endif
    }

    sys_Usec_Time();
    
    return ((rc1 == -1) || (rc2 == -1)) ? -1 : 0;
}

/***************************************************************************************
*函数：QuickUpdateLogFile
*描述：读取EM133两条报文，并将读取的数据写入到数据文件中；
*参数：
*    无
*返回值：成功返回0，否则返回-1。
***************************************************************************************/
int CEM133Collector::QuickUpdateLogFile() {
    if(m_Data_Cnt == MAX_DATA_CNT) {
         m_Data_Cnt = 0;
         m_File_Cnt++;
         if(m_File_Cnt == MAX_FILE_CNT + 1) {
             m_File_Cnt = 1;
         }
         Create_New_Log_File();
    }
    m_Data_Cnt++;
	int rc =ReadEM133SingleCMD();
    if(rc == 0) {
//         long lTotal_kW = m_tab_reg1[6] + m_tab_reg1[7] + m_tab_reg1[8];
//         long lTotal_kvar = m_tab_reg1[9] + m_tab_reg1[10] + m_tab_reg1[11];
//         long lTotal_kVA = m_tab_reg1[12] + m_tab_reg1[13] + m_tab_reg1[14];
//         long lTotal_PF = lTotal_kW / lTotal_kVA;
         //long lTotal_PF = sqrt((pow(m_tab_reg1[15],2) + pow(m_tab_reg1[16],2) + pow(m_tab_reg1[17], 2)) / 3);
         ofstream out;
         out.open(m_File_Name, ofstream::out | ofstream::app);
         if(out) {
             //采1条报文，自动计算总功率
//              out << m_Current_Time << "," << m_tab_reg1[0] << ","  << m_tab_reg1[1] << ","  << m_tab_reg1[2] << ","  
//                  << m_tab_reg1[3] << ","  << m_tab_reg1[4] << ","  << m_tab_reg1[5] << ","  
//                  << lTotal_kW << ","  << lTotal_kvar << ","  << lTotal_kVA << ","  << lTotal_PF << "," 
//                  << "\r\n";

              //采2条报文
//              out << m_Current_Time << "," << m_tab_reg1[0] << ","  << m_tab_reg1[1] << ","  << m_tab_reg1[2] << ","  
//                  << m_tab_reg1[3] << ","  << m_tab_reg1[4] << ","  << m_tab_reg1[5] << ","  
//                  << m_tab_reg2[0] << ","  << m_tab_reg2[1] << ","  << m_tab_reg2[2] << ","  << m_tab_reg2[3] << "," 
//                  << "\r\n";

              //采2条报文，写入所有数据
              int tmpFrequency = 0;
              out << m_Current_Time << "," << m_tab_reg1[0] << ","  << m_tab_reg1[1] << ","  << m_tab_reg1[2] << ","  
                  << m_tab_reg1[3] << ","  << m_tab_reg1[4] << ","  << m_tab_reg1[5] << ","  
                  << m_tab_reg2[0] << ","  << m_tab_reg2[1] << ","  << m_tab_reg2[2] << ","  << m_tab_reg2[3] << "," 
                  << tmpFrequency << ","
                  << m_tab_reg1[6] << ","  << m_tab_reg1[7] << ","  << m_tab_reg1[8] << ","  
                  << m_tab_reg1[9] << ","  << m_tab_reg1[10] << ","  << m_tab_reg1[11] << ","  
                  << m_tab_reg1[12] << ","  << m_tab_reg1[13] << ","  << m_tab_reg1[14] << ","  
                  << m_tab_reg1[15] << ","  << m_tab_reg1[16] << ","  << m_tab_reg1[17] << ","  
                  << "\r\n";
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
