#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "vsoa_server.h"
#include "vsoa_parser.h"
#include "vsoa_platform.h"
#include "computility.h"
#include "selfmath.h"
#include "AttiMeasFun.h"
#include "dataproc_server.h"
#include "dataproc_parser.h"
#include "dataproc.h"

#define SERVER_PASSWD       "123456"
#define SERVER_DEFAULT_PORT 33333

// 当前服务器实例
static vsoa_server_t *server;

/*
* /dataproc/gyro Callback
*/
void command_gyro(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
                        vsoa_header_t *vsoa_hdr, vsoa_url_t *url,
                        vsoa_payload_t *payload) {
    uint32_t seqno = vsoa_parser_get_seqno(vsoa_hdr);
    vsoa_payload_t send = {0};

    // 初始化变量
    FiberGyro Gyro1 = {0};
    MEMSGyro Gyro2 = {0};
    int OrbCtrlAllowFlag = 0;
    double wbi_StarMeas[3] = {0};

    // 解析客户端传入的 JSON 数据
    if (!parse_gyro_payload(payload, &Gyro1, &Gyro2, &OrbCtrlAllowFlag, wbi_StarMeas)) {
        send.param = "{\"error\": \"Invalid input data\"}";
        send.param_len = strlen(send.param);
        vsoa_server_cli_reply(server, cid, 1, seqno, 0, &send);
        return;
    }

    // // 打印解析后的参数
    // printf("=== Gyro Data Processing Parameters ===\n");
    // printf("Gyro1 - Flag: %d, OnFlag: %d, FaultCnt: %d\n", Gyro1.Flag, Gyro1.OnFlag, Gyro1.FaultCnt);
    // printf("Gyro1 - Meas: [%.6f, %.6f, %.6f]\n", Gyro1.Meas[0], Gyro1.Meas[1], Gyro1.Meas[2]);
    // printf("Gyro2 - Flag: %d, OnFlag: %d, FaultCnt: %d\n", Gyro2.Flag, Gyro2.OnFlag, Gyro2.FaultCnt);
    // printf("Gyro2 - Meas: [%.6f, %.6f, %.6f]\n", Gyro2.Meas[0], Gyro2.Meas[1], Gyro2.Meas[2]);
    // printf("OrbCtrlAllowFlag: %d\n", OrbCtrlAllowFlag);
    // printf("wbi_StarMeas: [%.6f, %.6f, %.6f]\n", wbi_StarMeas[0], wbi_StarMeas[1], wbi_StarMeas[2]);
    // printf("=======================================\n");

    // 执行数据处理
    GyroDataProcessing(Gyro1, Gyro2, OrbCtrlAllowFlag, wbi_StarMeas);

    // 发送简化的成功响应
    const char *result = "{\"status\": \"success\"}";
    
    send.param = result;
    send.param_len = strlen(result);
    vsoa_server_cli_reply(server, cid, 0, seqno, 0, &send);
}

/*
* /dataproc/star Callback
*/
void command_star(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
                         vsoa_header_t *vsoa_hdr, vsoa_url_t *url,
                         vsoa_payload_t *payload) {
    uint32_t seqno = vsoa_parser_get_seqno(vsoa_hdr);
    vsoa_payload_t send = {0};

    // Initialize variables
    StarSensor Star1 = {0};
    StarSensor Star2 = {0};
    AttitudeData AttiData = {0};

    // Parse client JSON data
    if (!parse_star_payload(payload, &Star1, &Star2, &AttiData)) {
        send.param = "{\"error\": \"Invalid input data\"}";
        send.param_len = strlen(send.param);
        vsoa_server_cli_reply(server, cid, 1, seqno, 0, &send);
        return;
    }

    // // 打印解析后的参数
    // printf("=== Star Data Processing Parameters ===\n");
    // printf("Star1 - Flag: %d, OnFlag: %d, FaultCnt: %d, StarNum: %d\n", Star1.Flag, Star1.OnFlag, Star1.FaultCnt, Star1.StarNum);
    // printf("Star1 - Meas: [%.6f, %.6f, %.6f, %.6f]\n", Star1.Meas[0], Star1.Meas[1], Star1.Meas[2], Star1.Meas[3]);
    // printf("Star1 - MeasVel: [%.6f, %.6f, %.6f]\n", Star1.MeasVel[0], Star1.MeasVel[1], Star1.MeasVel[2]);
    // printf("Star2 - Flag: %d, OnFlag: %d, FaultCnt: %d, StarNum: %d\n", Star2.Flag, Star2.OnFlag, Star2.FaultCnt, Star2.StarNum);
    // printf("Star2 - Meas: [%.6f, %.6f, %.6f, %.6f]\n", Star2.Meas[0], Star2.Meas[1], Star2.Meas[2], Star2.Meas[3]);
    // printf("Star2 - MeasVel: [%.6f, %.6f, %.6f]\n", Star2.MeasVel[0], Star2.MeasVel[1], Star2.MeasVel[2]);
    // printf("AttiData - AttiFiltValidFlag: %d, OrbCtrlAllowFlag: %d\n", AttiData.AttiFiltValidFlag, AttiData.OrbCtrlAllowFlag);
    // printf("AttiData - QuatOrbit: [%.6f, %.6f, %.6f, %.6f]\n", AttiData.QuatOrbit[0], AttiData.QuatOrbit[1], AttiData.QuatOrbit[2], AttiData.QuatOrbit[3]);
    // printf("AttiData - AttiQuatEst: [%.6f, %.6f, %.6f, %.6f]\n", AttiData.AttiQuatEst[0], AttiData.AttiQuatEst[1], AttiData.AttiQuatEst[2], AttiData.AttiQuatEst[3]);
    // printf("AttiData - wbi_StarMeas: [%.6f, %.6f, %.6f]\n", AttiData.wbi_StarMeas[0], AttiData.wbi_StarMeas[1], AttiData.wbi_StarMeas[2]);
    // printf("======================================\n");

    // Execute data processing
    StarDataProcessing(Star1, Star2, AttiData);

    // Send simplified success response
    const char *result = "{\"status\": \"success\"}";
    
    send.param = result;
    send.param_len = strlen(result);
    vsoa_server_cli_reply(server, cid, 0, seqno, 0, &send);
}

int main(int argc, char **argv) {
    // 绑定地址时确保正确性
    struct sockaddr_in addr;

    // 绑定到0.0.0.0:33333
    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_DEFAULT_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

#ifdef VSOA_HAS_SIN_LEN
    addr.sin_len = sizeof(struct sockaddr_in);
#endif

    /*
    * Initialize server
    */
    server = vsoa_server_create("{\"name\":\"dataproc_server\"}");
    if (!server) {
        fprintf(stderr, "Can not create VSOA server!\n");
        return (-1);
    }

    /*
    * If need password
    */
    vsoa_server_passwd(server, SERVER_PASSWD);

    /*
    * Add /dataproc/gyro listener
    */
    vsoa_url_t url_gyro;
    url_gyro.url = "/dataproc/gyro";
    url_gyro.url_len = strlen(url_gyro.url);
    vsoa_server_add_listener(server, &url_gyro, command_gyro, NULL);

    /*
    * Add /dataproc/star listener
    */
    vsoa_url_t url_star;
    url_star.url = "/dataproc/star";
    url_star.url_len = strlen(url_star.url);
    vsoa_server_add_listener(server, &url_star, command_star, NULL);

    /*
    * Start server
    */
    if (!vsoa_server_start(server, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
        vsoa_server_close(server);
        fprintf(stderr, "Can not start VSOA server!\n");
        return (-1);
    }

    fprintf(stdout, "Create server successful, Waiting for client input...\n");

    int cnt, max_fd;
    fd_set fds;
    struct timespec timeout = {1, 0};
    while (1) {
        // 清空文件描述符集合fds
        FD_ZERO(&fds);
        // 获取服务器相关的文件描述符并返回最大文件描述符
        max_fd = vsoa_server_fds(server, &fds);

        cnt = pselect(max_fd + 1, &fds, NULL, NULL, &timeout, NULL);
        if (cnt > 0) {
            vsoa_server_input_fds(server, &fds);
        }
    }

    return (0);
}

// int main() {
//     test_command_gyro_callback();
//     test_command_star_callback();

//     return 0;
// }
