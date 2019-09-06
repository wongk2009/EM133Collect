#include "CEM133Collector.h"

CEM133Collector::CEM133Collector(){
    //设置modbus超时时间为1000毫秒
    m_old_response_to_sec = 0;
    m_old_response_to_usec = 1000000;
}

CEM133Collector::~CEM133Collector(){

}

int CEM133Collector::SetUpTCPServer(const char *ip = "127.0.0.1", int port = 1502){
    uint16_t tab_reg[64];
    modbus_t *ctx;
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
    rc = modbus_read_registers(ctx, 2000, 10, tab_reg);
    if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }

    for (int i=0; i < rc; i++) {
         printf("reg[%d]=%d (0x%X)\n", i, tab_reg[i], tab_reg[i]);
    }

    modbus_close(ctx);
    modbus_free(ctx);
    return 0;
}
