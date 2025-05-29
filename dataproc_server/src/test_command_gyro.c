/*
 * test_command_gyro.c
 *
 * Test file for command_gyro callback function
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vsoa_server.h"
#include "vsoa_parser.h"
#include "dataproc_parser.h"
#include "computility.h"

// Mock test context to avoid global variable pollution
typedef struct {
    vsoa_cli_id_t last_cid; // Renamed from cid to avoid confusion with function parameters
    char last_response[1024];
    int last_error_code;
    // server_instance is removed as it's not used by the mock and caused issues
} test_context_t;

static test_context_t g_test_ctx = {0};

// Forward declaration for the mock function
bool mock_vsoa_server_cli_reply(vsoa_server_t *server, vsoa_cli_id_t id,
                           uint8_t status, uint32_t seqno, uint16_t tunid, const vsoa_payload_t *payload);

// Function pointer to our mock or the real function
bool (*vsoa_server_cli_reply_func)(vsoa_server_t *server, vsoa_cli_id_t id,
                                uint8_t status, uint32_t seqno, uint16_t tunid, const vsoa_payload_t *payload) = mock_vsoa_server_cli_reply;

// Mock implementation of vsoa_server_cli_reply
bool mock_vsoa_server_cli_reply(vsoa_server_t *server, vsoa_cli_id_t id,
                           uint8_t status, uint32_t seqno, uint16_t tunid, const vsoa_payload_t *payload) {
    g_test_ctx.last_error_code = status;
    if (payload && payload->param) {
        strncpy(g_test_ctx.last_response, payload->param, sizeof(g_test_ctx.last_response) - 1);
        g_test_ctx.last_response[sizeof(g_test_ctx.last_response) - 1] = '\0';
    }
    printf("Mock Reply - Error Code: %d, Response: %s\n", status, g_test_ctx.last_response);
    return true;
}

// Actual vsoa_server_cli_reply that will be linked, acting as a trampoline to our mock
bool vsoa_server_cli_reply(vsoa_server_t *server, vsoa_cli_id_t id, uint8_t status, uint32_t seqno, uint16_t tunid, const vsoa_payload_t *payload) {
    if (vsoa_server_cli_reply_func) {
        return vsoa_server_cli_reply_func(server, id, status, seqno, tunid, payload);
    }
    fprintf(stderr, "Error: vsoa_server_cli_reply_func not set!\n");
    return false;
}

// External declaration of command_gyro function
extern void command_gyro(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
                        vsoa_header_t *vsoa_hdr, vsoa_url_t *url,
                        vsoa_payload_t *payload);

// Test function for command_gyro with valid data
void test_command_gyro_valid_data() {
    printf("\n=== Testing command_gyro with valid data ===\n");
    
    // Initialize test context
    g_test_ctx.last_cid = 1;
    memset(g_test_ctx.last_response, 0, sizeof(g_test_ctx.last_response));
    g_test_ctx.last_error_code = -1;
    
    // Create test JSON payload
    const char *test_json = "{"
        "\"Gyro1\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"Meas\": [0.1, 0.2, 0.3],"
            "\"MtxInstall\": [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]"
        "},"
        "\"Gyro2\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"Meas\": [0.4, 0.5, 0.6],"
            "\"MtxInstall\": [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]"
        "},"
        "\"OrbCtrlAllowFlag\": 1"
    "}";
    
    // Setup test payload
    vsoa_payload_t payload = {0};
    payload.param = (char*)test_json;
    payload.param_len = strlen(test_json);
    
    // Setup mock header
    vsoa_header_t header = {0};
    header.seqno = htonl(12345);
    
    // Setup mock URL
    vsoa_url_t url = {0};
    
    // Call the function with NULL server (mock will handle the reply)
    command_gyro(NULL, NULL, g_test_ctx.last_cid, &header, &url, &payload);
    
    // Check results
    printf("Expected error code: 0, Actual: %d\n", g_test_ctx.last_error_code);
    printf("Response: %s\n", g_test_ctx.last_response);
    
    if (g_test_ctx.last_error_code == 0 && strstr(g_test_ctx.last_response, "success")) {
        printf("✓ Test PASSED: Valid data processed successfully\n");
    } else {
        printf("✗ Test FAILED: Expected success response\n");
    }
}

// Test function for command_gyro with invalid data
void test_command_gyro_invalid_data() {
    printf("\n=== Testing command_gyro with invalid data ===\n");
    
    // Initialize test context
    g_test_ctx.last_cid = 1;
    memset(g_test_ctx.last_response, 0, sizeof(g_test_ctx.last_response));
    g_test_ctx.last_error_code = -1;
    
    // Create invalid JSON payload (missing required fields)
    const char *test_json = "{"
        "\"Gyro1\": {"
            "\"Flag\": 1"
        "}"
    "}";
    
    // Setup test payload
    vsoa_payload_t payload = {0};
    payload.param = (char*)test_json;
    payload.param_len = strlen(test_json);
    
    // Setup mock header
    vsoa_header_t header = {0};
    header.seqno = htonl(12346);
    
    // Setup mock URL
    vsoa_url_t url = {0};
    
    // Call the function with NULL server (mock will handle the reply)
    command_gyro(NULL, NULL, g_test_ctx.last_cid, &header, &url, &payload);
    
    // Check results
    printf("Expected error code: 1, Actual: %d\n", g_test_ctx.last_error_code);
    printf("Response: %s\n", g_test_ctx.last_response);
    
    if (g_test_ctx.last_error_code == 1 && strstr(g_test_ctx.last_response, "error")) {
        printf("✓ Test PASSED: Invalid data handled correctly\n");
    } else {
        printf("✗ Test FAILED: Expected error response\n");
    }
}

// Test function for command_gyro with malformed JSON
void test_command_gyro_malformed_json() {
    printf("\n=== Testing command_gyro with malformed JSON ===\n");
    
    // Initialize test context
    g_test_ctx.last_cid = 1;
    memset(g_test_ctx.last_response, 0, sizeof(g_test_ctx.last_response));
    g_test_ctx.last_error_code = -1;
    
    // Create malformed JSON payload
    const char *test_json = "{invalid json}";
    
    // Setup test payload
    vsoa_payload_t payload = {0};
    payload.param = (char*)test_json;
    payload.param_len = strlen(test_json);
    
    // Setup mock header
    vsoa_header_t header = {0};
    header.seqno = htonl(12347);
    
    // Setup mock URL
    vsoa_url_t url = {0};
    
    // Call the function with NULL server (mock will handle the reply)
    command_gyro(NULL, NULL, g_test_ctx.last_cid, &header, &url, &payload);
    
    // Check results
    printf("Expected error code: 1, Actual: %d\n", g_test_ctx.last_error_code);
    printf("Response: %s\n", g_test_ctx.last_response);
    
    if (g_test_ctx.last_error_code == 1 && strstr(g_test_ctx.last_response, "error")) {
        printf("✓ Test PASSED: Malformed JSON handled correctly\n");
    } else {
        printf("✗ Test FAILED: Expected error response\n");
    }
}

// Test parse_gyro_payload function directly
bool test_parse_gyro_payload() {
    printf("\n=== Testing parse_gyro_payload function directly ===\n");

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
    payload.param = (char*)test_json;
    payload.param_len = strlen(test_json);

    // 声明模拟输入参数
    FiberGyro Gyro1 = {0};
    MEMSGyro Gyro2 = {0};
    int OrbCtrlAllowFlag = 0;

    // 调用被测试函数
    bool result = parse_gyro_payload(&payload, &Gyro1, &Gyro2, &OrbCtrlAllowFlag);

    // 验证函数返回值
    if (!result) {
        printf("✗ Test FAILED: Function returned false\n");
        return false;
    }

    // 打印函数返回值
    printf("Parse result: SUCCESS\n");
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
        printf("✗ Test FAILED: Gyro1 data mismatch\n");
        return false;
    }

    if (Gyro2.Flag != 1 || Gyro2.Meas[0] != 0.15 || Gyro2.Meas[1] != 0.25 || Gyro2.Meas[2] != 0.35) {
        printf("✗ Test FAILED: Gyro2 data mismatch\n");
        return false;
    }

    if (OrbCtrlAllowFlag != 1) {
        printf("✗ Test FAILED: OrbCtrlAllowFlag mismatch\n");
        return false;
    }

    printf("✓ Test PASSED: parse_gyro_payload works correctly\n");
    return true;
}

// Test parse_gyro_payload error cases
bool test_parse_gyro_payload_error_cases() {
    printf("\n=== Testing parse_gyro_payload error cases ===\n");

    // 缺少必要字段
    const char *invalid_json1 = "{\"Gyro1\":{\"Flag\":1},\"OrbCtrlAllowFlag\":1}";
    vsoa_payload_t payload1 = {0};
    payload1.param = (char*)invalid_json1;
    payload1.param_len = strlen(invalid_json1);

    FiberGyro Gyro1 = {0};
    MEMSGyro Gyro2 = {0};
    int OrbCtrlAllowFlag = 0;

    bool result1 = parse_gyro_payload(&payload1, &Gyro1, &Gyro2, &OrbCtrlAllowFlag);
    printf("Missing required fields: %s\n", result1 ? "FAILED (should be error)" : "PASSED (correctly failed)");
    if (result1) {
        printf("✗ Test FAILED: Should report missing Gyro2 field\n");
        return false;
    }

    // JSON格式错误
    const char *invalid_json2 = "{\"Gyro1\":{\"Flag\":1},\"Gyro2\":{\"Flag\":1},\"OrbCtrlAllowFlag\":1,}";
    vsoa_payload_t payload2 = {0};
    payload2.param = (char*)invalid_json2;
    payload2.param_len = strlen(invalid_json2);

    bool result2 = parse_gyro_payload(&payload2, &Gyro1, &Gyro2, &OrbCtrlAllowFlag);
    printf("JSON format error: %s\n", result2 ? "FAILED (should be error)" : "PASSED (correctly failed)");
    if (result2) {
        printf("✗ Test FAILED: Should report JSON format error\n");
        return false;
    }

    // 字段类型错误
    const char *invalid_json3 = "{\"Gyro1\":{\"Flag\":\"not_a_number\"},\"Gyro2\":{\"Flag\":1},\"OrbCtrlAllowFlag\":1}";
    vsoa_payload_t payload3 = {0};
    payload3.param = (char*)invalid_json3;
    payload3.param_len = strlen(invalid_json3);

    bool result3 = parse_gyro_payload(&payload3, &Gyro1, &Gyro2, &OrbCtrlAllowFlag);
    printf("Field type error: %s\n", result3 ? "PASSED (type conversion)" : "FAILED (strict type check)");

    printf("✓ Test PASSED: Error cases handled correctly\n");
    return true;
}

void test_command_gyro_callback(){
    printf("Starting command_gyro callback function tests...\n");
    
    test_parse_gyro_payload();
    test_parse_gyro_payload_error_cases();
    test_command_gyro_valid_data();
    test_command_gyro_invalid_data();
    test_command_gyro_malformed_json();
    
    printf("\n=== All tests completed ===\n");
}
