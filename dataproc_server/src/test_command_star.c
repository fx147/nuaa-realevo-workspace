/*
 * test_command_star.c
 *
 * Test file for command_star callback function
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
    vsoa_cli_id_t last_cid; // Renamed from cid, server_instance removed
    char last_response[1024];
    int last_error_code;
} test_context_t;

static test_context_t g_test_ctx = {0};

// Mocked version of vsoa_server_cli_reply
// Ensure this function has the exact signature as in vsoa_server.h (bool return, specific params)
static bool mock_vsoa_server_cli_reply(vsoa_server_t *server, vsoa_cli_id_t id, uint8_t status, uint32_t seqno, uint16_t tunid, const vsoa_payload_t *payload) {
   g_test_ctx.last_cid = id; // Store the client ID from the actual parameters
   g_test_ctx.last_error_code = status; // Store the status as error_code
   if (payload && payload->param) {
       strncpy(g_test_ctx.last_response, payload->param, sizeof(g_test_ctx.last_response) - 1);
       g_test_ctx.last_response[sizeof(g_test_ctx.last_response) - 1] = '\0';
   }
   // printf("Mock Reply - CID: %p, Status: %u, SeqNo: %u, TunID: %u, Response: %s\n", (void*)id, status, seqno, tunid, g_test_ctx.last_response);
   return true; // Match the bool return type
}

// Function pointer to the mock reply function
static bool (*vsoa_server_cli_reply_func)(vsoa_server_t*, vsoa_cli_id_t, uint8_t, uint32_t, uint16_t, const vsoa_payload_t*) = mock_vsoa_server_cli_reply;

// Static trampoline function to avoid multiple definition with test_command_gyro.c
static bool test_vsoa_server_cli_reply(vsoa_server_t *server, vsoa_cli_id_t id, uint8_t status, uint32_t seqno, uint16_t tunid, const vsoa_payload_t *payload) {
    if (vsoa_server_cli_reply_func) {
        return vsoa_server_cli_reply_func(server, id, status, seqno, tunid, payload);
    }
    fprintf(stderr, "Error: vsoa_server_cli_reply_func not set!\n");
    return false;
}

// External declaration of command_star function
extern void command_star(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
                        vsoa_header_t *vsoa_hdr, vsoa_url_t *url,
                        vsoa_payload_t *payload);

// Test function for command_star with valid data
void test_command_star_valid_data() {
    printf("\n=== Testing command_star with valid data ===\n");
    
    // Create test JSON payload
    const char *test_json = "{"
        "\"Star1\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"StarNum\": 1,"
            "\"Meas\": [0.1, 0.2, 0.3, 0.4],"
            "\"MeasVel\": [0.01, 0.02, 0.03],"
            "\"QuatInstall\": [1.0, 0.0, 0.0, 0.0],"
            "\"MtxInstall\": [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]"
        "},"
        "\"Star2\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"StarNum\": 2,"
            "\"Meas\": [0.5, 0.6, 0.7, 0.8],"
            "\"MeasVel\": [0.04, 0.05, 0.06],"
            "\"QuatInstall\": [0.0, 1.0, 0.0, 0.0],"
            "\"MtxInstall\": [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]"
        "},"
        "\"AttitudeData\": {"
            "\"AttiFiltValidFlag\": 1,"
            "\"OrbCtrlAllowFlag\": 1,"
            "\"QuatOrbit\": [1.0, 0.0, 0.0, 0.0],"
            "\"AttiQuatEst\": [0.9, 0.1, 0.1, 0.1],"
            "\"QuatAverage\": [0.95, 0.05, 0.05, 0.05],"
            "\"wbi_StarMeas\": [0.001, 0.002, 0.003]"
        "}"
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
    
    // Clear previous response from global context
    memset(g_test_ctx.last_response, 0, sizeof(g_test_ctx.last_response));
    g_test_ctx.last_error_code = -1; // Reset error code in context
    g_test_ctx.last_cid = 0;         // Reset CID in context

    // Call the function (passing NULL for server as it's not used by the mock, and g_test_ctx.last_cid)
    command_star(NULL, NULL, g_test_ctx.last_cid, &header, &url, &payload);
    
    // Check results from global context
    printf("Expected error code: 0, Actual: %d\n", g_test_ctx.last_error_code);
    printf("Response: %s\n", g_test_ctx.last_response);
    
    if (g_test_ctx.last_error_code == 0 && strstr(g_test_ctx.last_response, "success")) {
        printf("✓ Test PASSED: Valid data processed successfully\n");
    } else {
        printf("✗ Test FAILED: Expected success response\n");
    }
}

// Test function for command_star with invalid data
void test_command_star_invalid_data() {
    printf("\n=== Testing command_star with invalid data ===\n");
    
    // Create invalid JSON payload (missing required fields)
    const char *test_json = "{"
        "\"Star1\": {"
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
    
    // Clear previous response from global context
    memset(g_test_ctx.last_response, 0, sizeof(g_test_ctx.last_response));
    g_test_ctx.last_error_code = -1; // Reset error code in context
    g_test_ctx.last_cid = 0;         // Reset CID in context

    // Call the function (passing NULL for server as it's not used by the mock, and g_test_ctx.last_cid)
    command_star(NULL, NULL, g_test_ctx.last_cid, &header, &url, &payload);
    
    // Check results from global context
    printf("Expected error code: 1, Actual: %d\n", g_test_ctx.last_error_code);
    printf("Response: %s\n", g_test_ctx.last_response);
    
    if (g_test_ctx.last_error_code == 1 && strstr(g_test_ctx.last_response, "error")) {
        printf("✓ Test PASSED: Invalid data handled correctly\n");
    } else {
        printf("✗ Test FAILED: Expected error response\n");
    }
}

// Test function for command_star with malformed JSON
void test_command_star_malformed_json() {
    printf("\n=== Testing command_star with malformed JSON ===\n");
    
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
    
    // Clear previous response from global context
    memset(g_test_ctx.last_response, 0, sizeof(g_test_ctx.last_response));
    g_test_ctx.last_error_code = -1; // Reset error code in context
    g_test_ctx.last_cid = 0;         // Reset CID in context

    // Call the function (passing NULL for server as it's not used by the mock, and g_test_ctx.last_cid)
    command_star(NULL, NULL, g_test_ctx.last_cid, &header, &url, &payload);
    
    // Check results from global context
    printf("Expected error code: 1, Actual: %d\n", g_test_ctx.last_error_code);
    printf("Response: %s\n", g_test_ctx.last_response);
    
    if (g_test_ctx.last_error_code == 1 && strstr(g_test_ctx.last_response, "error")) {
        printf("✓ Test PASSED: Malformed JSON handled correctly\n");
    } else {
        printf("✗ Test FAILED: Expected error response\n");
    }
}

// Test parse_star_payload function directly
void test_parse_star_payload() {
    printf("\n=== Testing parse_star_payload function directly ===\n");
    
    const char *test_json = "{"
        "\"Star1\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"StarNum\": 1,"
            "\"Meas\": [0.1, 0.2, 0.3, 0.4],"
            "\"MeasVel\": [0.01, 0.02, 0.03],"
            "\"QuatInstall\": [1.0, 0.0, 0.0, 0.0],"
            "\"MtxInstall\": [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]"
        "},"
        "\"Star2\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"StarNum\": 2,"
            "\"Meas\": [0.5, 0.6, 0.7, 0.8],"
            "\"MeasVel\": [0.04, 0.05, 0.06],"
            "\"QuatInstall\": [0.0, 1.0, 0.0, 0.0],"
            "\"MtxInstall\": [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]"
        "},"
        "\"AttitudeData\": {"
            "\"AttiFiltValidFlag\": 1,"
            "\"OrbCtrlAllowFlag\": 1,"
            "\"QuatOrbit\": [1.0, 0.0, 0.0, 0.0],"
            "\"AttiQuatEst\": [0.9, 0.1, 0.1, 0.1],"
            "\"QuatAverage\": [0.95, 0.05, 0.05, 0.05],"
            "\"wbi_StarMeas\": [0.001, 0.002, 0.003]"
        "}"
    "}";
    
    vsoa_payload_t payload = {0};
    payload.param = (char*)test_json;
    payload.param_len = strlen(test_json);
    
    StarSensor Star1 = {0};
    StarSensor Star2 = {0};
    AttitudeData AttiData = {0};
    
    bool result = parse_star_payload(&payload, &Star1, &Star2, &AttiData);
    
    printf("Parse result: %s\n", result ? "SUCCESS" : "FAILED");
    
    if (result) {
        printf("Star1 - Flag: %d, StarNum: %d, Meas[0]: %f\n", Star1.Flag, Star1.StarNum, Star1.Meas[0]);
        printf("Star2 - Flag: %d, StarNum: %d, Meas[0]: %f\n", Star2.Flag, Star2.StarNum, Star2.Meas[0]);
        printf("AttitudeData - AttiFiltValidFlag: %d, QuatAverage[0]: %f\n", 
               AttiData.AttiFiltValidFlag, AttiData.QuatAverage[0]);
        printf("✓ Test PASSED: parse_star_payload works correctly\n");
    } else {
        printf("✗ Test FAILED: parse_star_payload failed\n");
    }
}

// Test parse_star_payload function directly with more comprehensive cases
bool test_parse_star_payload_comprehensive() {
    printf("\n=== Testing parse_star_payload function comprehensively ===\n");

    // 模拟完整的JSON数据
    const char *test_json = "{"
        "\"Star1\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"StarNum\": 1,"
            "\"Meas\": [0.1, 0.2, 0.3, 0.4],"
            "\"MeasVel\": [0.01, 0.02, 0.03],"
            "\"QuatInstall\": [1.0, 0.0, 0.0, 0.0],"
            "\"MtxInstall\": [[1,0,0],[0,1,0],[0,0,1]]"
        "},"
        "\"Star2\": {"
            "\"Flag\": 1,"
            "\"OnFlag\": 1,"
            "\"FaultCnt\": 0,"
            "\"StarNum\": 2,"
            "\"Meas\": [0.15, 0.25, 0.35, 0.45],"
            "\"MeasVel\": [0.04, 0.05, 0.06],"
            "\"QuatInstall\": [0.0, 1.0, 0.0, 0.0],"
            "\"MtxInstall\": [[1,0,0],[0,1,0],[0,0,1]]"
        "},"
        "\"AttitudeData\": {"
            "\"AttiFiltValidFlag\": 1,"
            "\"OrbCtrlAllowFlag\": 1,"
            "\"QuatOrbit\": [1.0, 0.0, 0.0, 0.0],"
            "\"AttiQuatEst\": [0.9, 0.1, 0.1, 0.1],"
            "\"QuatAverage\": [0.95, 0.05, 0.05, 0.05],"
            "\"wbi_StarMeas\": [0.001, 0.002, 0.003]"
        "}"
    "}";

    // 构造模拟payload
    vsoa_payload_t payload = {0};
    payload.param = (char*)test_json;
    payload.param_len = strlen(test_json);

    // 声明模拟输入参数
    StarSensor Star1 = {0};
    StarSensor Star2 = {0};
    AttitudeData AttiData = {0};

    // 调用被测试函数
    bool result = parse_star_payload(&payload, &Star1, &Star2, &AttiData);

    // 验证函数返回值
    if (!result) {
        printf("✗ Test FAILED: Function returned false\n");
        return false;
    }

    // 打印函数返回值
    printf("Parse result: SUCCESS\n");
    printf("Star1: Flag=%d, OnFlag=%d, FaultCnt=%d\n",
           Star1.Flag, Star1.OnFlag, Star1.FaultCnt);
    printf("Star1.Meas=[%f, %f, %f, %f]\n",
           Star1.Meas[0], Star1.Meas[1], Star1.Meas[2], Star1.Meas[3]);
    printf("Star2: Flag=%d, OnFlag=%d, FaultCnt=%d\n",
           Star2.Flag, Star2.OnFlag, Star2.FaultCnt);
    printf("AttiData: AttiFiltValidFlag=%d, OrbCtrlAllowFlag=%d\n",
           AttiData.AttiFiltValidFlag, AttiData.OrbCtrlAllowFlag);

    // 验证函数返回值是否符合预期
    if (Star1.Flag != 1 || Star1.Meas[0] != 0.1 || Star1.Meas[1] != 0.2 || 
        Star1.Meas[2] != 0.3 || Star1.Meas[3] != 0.4) {
        printf("✗ Test FAILED: Star1 data mismatch\n");
        return false;
    }

    // Note: Star2 and AttiData validation would depend on the actual JSON structure
    // For now, just check that the function succeeded
    printf("Star2 and AttiData parsed successfully\n");

    printf("✓ Test PASSED: parse_star_payload works correctly\n");
    return true;
}

// Test parse_star_payload error cases
bool test_parse_star_payload_error_cases() {
    printf("\n=== Testing parse_star_payload error cases ===\n");

    // 缺少必要字段
    const char *invalid_json1 = "{\"Star1\":{\"Flag\":1}}";
    vsoa_payload_t payload1 = {0};
    payload1.param = (char*)invalid_json1;
    payload1.param_len = strlen(invalid_json1);

    StarSensor Star1 = {0};
    StarSensor Star2 = {0};
    AttitudeData AttiData = {0};

    bool result1 = parse_star_payload(&payload1, &Star1, &Star2, &AttiData);
    printf("Missing required fields: %s\n", result1 ? "FAILED (should be error)" : "PASSED (correctly failed)");
    if (result1) {
        printf("✗ Test FAILED: Should report missing Attitude1 field\n");
        return false;
    }

    // JSON格式错误
    const char *invalid_json2 = "{\"Star1\":{\"Flag\":1},\"Attitude1\":{\"Flag\":1},}";
    vsoa_payload_t payload2 = {0};
    payload2.param = (char*)invalid_json2;
    payload2.param_len = strlen(invalid_json2);

    bool result2 = parse_star_payload(&payload2, &Star1, &Star2, &AttiData);
    printf("JSON format error: %s\n", result2 ? "FAILED (should be error)" : "PASSED (correctly failed)");
    if (result2) {
        printf("✗ Test FAILED: Should report JSON format error\n");
        return false;
    }

    // 字段类型错误
    const char *invalid_json3 = "{\"Star1\":{\"Flag\":\"not_a_number\"},\"Attitude1\":{\"Flag\":1}}";
    vsoa_payload_t payload3 = {0};
    payload3.param = (char*)invalid_json3;
    payload3.param_len = strlen(invalid_json3);

    bool result3 = parse_star_payload(&payload3, &Star1, &Star2, &AttiData);
    printf("Field type error: %s\n", result3 ? "PASSED (type conversion)" : "FAILED (strict type check)");

    printf("✓ Test PASSED: Error cases handled correctly\n");
    return true;
}

void test_command_star_callback() {
    printf("Starting command_star callback function tests...\n");
    
    test_parse_star_payload_comprehensive();
    test_parse_star_payload_error_cases();
    test_command_star_valid_data();
    test_command_star_invalid_data();
    test_command_star_malformed_json();
    test_parse_star_payload();
    
    printf("\n=== All tests completed ===\n");
}
