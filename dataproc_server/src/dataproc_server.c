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

    // 解析客户端传入的 JSON 数据
    if (!parse_gyro_payload(payload, &Gyro1, &Gyro2, &OrbCtrlAllowFlag)) {
        send.param = "{\"error\": \"Invalid input data\"}";
        send.param_len = strlen(send.param);
        vsoa_server_cli_reply(server, cid, 1, seqno, 0, &send);
        return;
    }

    double wbi_StarMeas[3] = {0};  // 默认值，表示未获取

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
