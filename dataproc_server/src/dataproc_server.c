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
#define VSOA_AUTO_PORT_ENV  "VSOA_AUTO_PORT="
#define SERVER_DEFAULT_PORT 3001

// 当前服务器实例
static vsoa_server_t *server;

/*
* /dataproc/gyro Callback
*/
static void command_gyro(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
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

    // 发送回应
    char result[512];
    snprintf(result, sizeof(result),
            "{\"wbi_meas\": [%f, %f, %f]}",
            Gyro1.Meas[0], Gyro1.Meas[1], Gyro1.Meas[2]);

    send.param = result;
    send.param_len = strlen(result);
    vsoa_server_cli_reply(server, cid, 0, seqno, 0, &send);
}

/*
* /dataproc/star Callback
*/
static void command_star(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
                         vsoa_header_t *vsoa_hdr, vsoa_url_t *url,
                         vsoa_payload_t *payload) {
    // uint32_t seqno = vsoa_parser_get_seqno(vsoa_hdr);
    // vsoa_payload_t send;
    // send.data = NULL;
    // send.data_len = 0;

    // // 初始化传感器数据
    // StarSensor Star1, Star2;
    // AttitudeData Attitude;

    // // 初始化传感器数据格式为 JSON，需要解析
    // // 如果解析失败则直接使用默认数据
    // init_star_sensor_data(&Star1, &Star2);
    // init_attitude_data(&Attitude);

    // // 执行数据处理
    // StarDataProcessing(Star1, Star2, Attitude);

    // // 构造返回结果
    // char result[256];
    // snprintf(result, sizeof(result), "{\"qbi_meas\": [%f, %f, %f, %f], \"StarValidFlag\": %d}",
    //          Attitude.QuatAverage[0], Attitude.QuatAverage[1],
    //          Attitude.QuatAverage[2], Attitude.QuatAverage[3],
    //          Attitude.StarValidFlag);

    // send.param = result;
    // send.param_len = strlen(result);

    // // 发送客户端
    // vsoa_server_cli_reply(server, cid, 0, seqno, 0, &send);
}

// 测试 parse_gyro_payload 函数
bool test_parse_gyro_payload() {
    printf("开始测试parse_gyro_payload函数...\n");

    // 模拟JSON数据
    const char *test_json = "{"
        "\"Gyro1\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"Meas\": [0.1, 0.2, 0.3],"
            "\"MtxInstall\": [[1,0,0],[0,1,0],[0,0,1]]"
        "},"
        "\"Gyro2\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"Meas\": [0.15, 0.25, 0.35],"
            "\"MtxInstall\": [[1,0,0],[0,1,0],[0,0,1]]"
        "},"
        "\"OrbCtrlAllowFlag\": 1"
    "}";

    // 构造模拟payload
    vsoa_payload_t payload = {0};
    payload.param = test_json;
    payload.param_len = strlen(test_json);

    // 声明模拟输入参数
    FiberGyro Gyro1 = {0};
    MEMSGyro Gyro2 = {0};
    int OrbCtrlAllowFlag = 0;

    // 调用被测试函数
    bool result = parse_gyro_payload(&payload, &Gyro1, &Gyro2, &OrbCtrlAllowFlag);

    // 验证函数返回值
    if (!result) {
        printf("测试失败: 函数返回false\n");
        return false;
    }

    // 打印函数返回值
    printf("函数返回值:\n");
    printf("Gyro1: Flag=%d, OnFlag=%d, FaultCnt=%d\n",
           Gyro1.Flag, Gyro1.OnFlag, Gyro1.FaultCnt);
    printf("Gyro1.Meas=[%f, %f, %f]\n",
           Gyro1.Meas[0], Gyro1.Meas[1], Gyro1.Meas[2]);
    printf("Gyro2: Flag=%d, OnFlag=%d, FaultCnt=%d\n",
           Gyro2.Flag, Gyro2.OnFlag, Gyro2.FaultCnt);
    printf("Gyro2.Meas=[%f, %f, %f]\n",
           Gyro2.Meas[0], Gyro2.Meas[1], Gyro2.Meas[2]);
    printf("OrbCtrlAllowFlag=%d\n", OrbCtrlAllowFlag);

    // 验证函数返回值是否符合预期
    if (Gyro1.Flag != 1 || Gyro1.Meas[0] != 0.1 || Gyro1.Meas[1] != 0.2 || Gyro1.Meas[2] != 0.3) {
        printf("测试失败: Gyro1数据不符\n");
        return false;
    }

    if (Gyro2.Flag != 1 || Gyro2.Meas[0] != 0.15 || Gyro2.Meas[1] != 0.25 || Gyro2.Meas[2] != 0.35) {
        printf("测试失败: Gyro2数据不符\n");
        return false;
    }

    if (OrbCtrlAllowFlag != 1) {
        printf("测试失败: OrbCtrlAllowFlag不符\n");
        return false;
    }

    printf("测试成功: 输入数据解析正确!\n");
    return true;
}

// 测试 parse_gyro_payload 错误情况
bool test_parse_gyro_payload_error_cases() {
    printf("\n开始测试错误情况...\n");

    // 缺少必要字段
    const char *invalid_json1 = "{\"Gyro1\":{\"Flag\":1},\"OrbCtrlAllowFlag\":1}";
    vsoa_payload_t payload1 = {0};
    payload1.param = invalid_json1;
    payload1.param_len = strlen(invalid_json1);

    FiberGyro Gyro1 = {0};
    MEMSGyro Gyro2 = {0};
    int OrbCtrlAllowFlag = 0;

    bool result1 = parse_gyro_payload(&payload1, &Gyro1, &Gyro2, &OrbCtrlAllowFlag);
    printf("缺少必要字段: %s\n", result1 ? "测试成功(错误)" : "测试失败(正确)");
    if (result1) {
        printf("测试失败: 应该报错Gyro2字段\n");
        return false;
    }

    // JSON格式错误
    const char *invalid_json2 = "{\"Gyro1\":{\"Flag\":1},\"Gyro2\":{\"Flag\":1},\"OrbCtrlAllowFlag\":1,}";
    vsoa_payload_t payload2 = {0};
    payload2.param = invalid_json2;
    payload2.param_len = strlen(invalid_json2);

    bool result2 = parse_gyro_payload(&payload2, &Gyro1, &Gyro2, &OrbCtrlAllowFlag);
    printf("JSON格式错误: %s\n", result2 ? "测试成功(错误)" : "测试失败(正确)");
    if (result2) {
        printf("测试失败: 应该报错JSON格式错误\n");
        return false;
    }

    // 字段类型错误
    const char *invalid_json3 = "{\"Gyro1\":{\"Flag\":\"not_a_number\"},\"Gyro2\":{\"Flag\":1},\"OrbCtrlAllowFlag\":1}";
    vsoa_payload_t payload3 = {0};
    payload3.param = invalid_json3;
    payload3.param_len = strlen(invalid_json3);

    bool result3 = parse_gyro_payload(&payload3, &Gyro1, &Gyro2, &OrbCtrlAllowFlag);
    printf("字段类型错误: %s\n", result3 ? "测试成功(错误类型)" : "测试失败(正确类型)");

    printf("测试错误情况结束\n");
    return true;
}

// 原始main函数注释掉
/*
int main(int argc, char **argv) {
    // 绑定地址时确保正确性
    struct sockaddr_in addr;
    uint16_t server_port = SERVER_DEFAULT_PORT;

    char *autoPort = getenv(VSOA_AUTO_PORT_ENV);
    if (autoPort != NULL) {
        fprintf(stdout, "ser port is %s .\n", autoPort);

        server_port = atoi(autoPort);
        if (server_port == 0) {
            server_port = SERVER_DEFAULT_PORT;
        }
    }

    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    addr.sin_addr.s_addr = INADDR_ANY;

#ifdef VSOA_HAS_SIN_LEN
    addr.sin_len = sizeof(struct sockaddr_in);
#endif

    /*
    * Initialize server
    */
/*
    server = vsoa_server_create("{\"name\":\"dataproc_server\"}");
    if (!server) {
        fprintf(stderr, "Can not create VSOA server!\n");
        return (-1);
    }

    /*
    * If need password
    */
/*
    vsoa_server_passwd(server, SERVER_PASSWD);

    /*
    * Add /dataproc/gyro listener
    */
/*
    vsoa_url_t url_gyro;
    url_gyro.url = "/dataproc/gyro";
    url_gyro.url_len = strlen(url_gyro.url);
    vsoa_server_add_listener(server, &url_gyro, command_gyro, NULL);

    /*
    * Add /dataproc/star listener
    */
/*
    vsoa_url_t url_star;
    url_star.url = "/dataproc/star";
    url_star.url_len = strlen(url_star.url);
    vsoa_server_add_listener(server, &url_star, command_star, NULL);

    /*
    * Start server
    */
/*
    if (!vsoa_server_start(server, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
        vsoa_server_close(server);
        fprintf(stderr, "Can not start VSOA server!\n");
        return (-1);
    }

    int cnt, max_fd;
    fd_set fds;
    struct timespec timeout = {1, 0};
    while (1) {
        FD_ZERO(&fds);
        max_fd = vsoa_server_fds(server, &fds);

        cnt = pselect(max_fd + 1, &fds, NULL, NULL, &timeout, NULL);
        if (cnt > 0) {
            vsoa_server_input_fds(server, &fds);
        }
    }

    return (0);
}
*/

// 修改：在 main 函数中设置 UTF-8 编码
int main(int argc, char **argv) {
    printf("======= 开始JSON解析测试 =======\n\n");

    // 测试正常情况
    bool normal_test_result = test_parse_gyro_payload();

    // 测试错误情况
    bool error_test_result = test_parse_gyro_payload_error_cases();

    printf("\n======= 结束测试 =======\n");
    printf("正常情况测试: %s\n", normal_test_result ? "通过" : "失败");
    printf("错误情况测试: %s\n", error_test_result ? "通过" : "失败");
    printf("总测试结果: %s\n", (normal_test_result && error_test_result) ? "通过" : "失败");

    return (normal_test_result && error_test_result) ? 0 : -1;
}
