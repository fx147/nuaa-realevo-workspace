/*
 * dataproc_client.c - Data Processing Client
 * 
 * This client sends RPC calls to dataproc_server for gyro and star data processing
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "vsoa_client.h"
#include "vsoa_position.h"
#include "vsoa_platform.h"

/* Server password */
#define SERVER_PASSWD "123456"

/* Client instance */
static vsoa_client_t *client;

/*
 * Gyro RPC callback
 */
static void gyro_callback(void *arg, struct vsoa_client *client, vsoa_header_t *vsoa_hdr, vsoa_payload_t *payload)
{
    if (!vsoa_hdr) {
        fprintf(stderr, "VSOA server /dataproc/gyro reply timeout!\n");
        return;
    }

    printf("Gyro processing result: %.*s\n", (int)payload->param_len, payload->param);
}

/*
 * Star RPC callback
 */
static void star_callback(void *arg, struct vsoa_client *client, vsoa_header_t *vsoa_hdr, vsoa_payload_t *payload)
{
    if (!vsoa_hdr) {
        fprintf(stderr, "VSOA server /dataproc/star reply timeout!\n");
        return;
    }

    printf("Star processing result: %.*s\n", (int)payload->param_len, payload->param);
}

/*
 * On message handler
 */
static void onmessage(void *arg, struct vsoa_client *client, vsoa_url_t *url, vsoa_payload_t *payload, bool quick)
{
    printf("On message, URL: %.*s payload: %.*s\n",
           (int)url->url_len, url->url, (int)payload->param_len, payload->param);
}

/*
 * Send gyro data for processing
 */
static bool send_gyro_data()
{
    vsoa_url_t url;
    vsoa_payload_t payload;
    struct timespec timeout = {5, 0};
    
    // Sample gyro JSON data
    const char *gyro_json = 
        "{"
            "\"Gyro1\": {"
                "\"Flag\": 1,"
                "\"Meas\": [0.001, 0.002, 0.003],"
                "\"MtxInstall\": [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],"
                "\"FaultCnt\": 0,"
                "\"OnFlag\": 1"
            "},"
            "\"Gyro2\": {"
                "\"Flag\": 1,"
                "\"Meas\": [0.004, 0.005, 0.006],"
                "\"MtxInstall\": [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]],"
                "\"FaultCnt\": 0,"
                "\"OnFlag\": 1"
            "},"
            "\"OrbCtrlAllowFlag\": 1"
        "}";
    
    url.url = "/dataproc/gyro";
    url.url_len = strlen(url.url);
    
    payload.data = NULL;
    payload.data_len = 0;
    payload.param = (char*)gyro_json;
    payload.param_len = strlen(gyro_json);
    
    printf("Sending gyro data: %s\n", gyro_json);
    
    return vsoa_client_call(client, VSOA_CLIENT_RPC_METHOD_SET,
                           &url, &payload, gyro_callback, NULL, &timeout);
}

/*
 * Send star data for processing
 */
static bool send_star_data()
{
    vsoa_url_t url;
    vsoa_payload_t payload;
    struct timespec timeout = {5, 0};
    
    // Sample star JSON data
    const char *star_json = 
        "{"
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
    
    url.url = "/dataproc/star";
    url.url_len = strlen(url.url);
    
    payload.data = NULL;
    payload.data_len = 0;
    payload.param = (char*)star_json;
    payload.param_len = strlen(star_json);
    
    printf("Sending star data: %s\n", star_json);
    
    return vsoa_client_call(client, VSOA_CLIENT_RPC_METHOD_SET,
                           &url, &payload, star_callback, NULL, &timeout);
}

/*
 * Main function
 */
int main(int argc, char **argv)
{
    int max_fd, cnt;
    fd_set fds;
    char info[256];
    socklen_t serv_len = sizeof(struct sockaddr_in);
    struct sockaddr_in addr;
    struct timespec timeout = {0, VSOA_CLIENT_TIMER_PERIOD * 1000000};
    
    printf("Data Processing Client Starting...\n");
    
    // Lookup server
    if (!vsoa_position_lookup(AF_INET, "dataproc_server",
                              (struct sockaddr *)&addr, &serv_len, NULL, &timeout)) {
        fprintf(stderr, "Can not found VSOA dataproc_server!\n");
        return -1;
    }
    
    // Create client
    client = vsoa_client_create(onmessage, NULL);
    if (!client) {
        fprintf(stderr, "Can not create VSOA client!\n");
        return -1;
    }
    
    // Connect to server
    if (!vsoa_client_connect(client, (struct sockaddr *)&addr, sizeof(struct sockaddr_in),
                             &timeout, SERVER_PASSWD, info, sizeof(info))) {
        vsoa_client_close(client);
        fprintf(stderr, "Can not connect to VSOA server!\n");
        return -1;
    }
    
    printf("Server (%s) connected\n", info);
    
    // Main loop - send data every 10ms
    while (1) {
        FD_ZERO(&fds);
        max_fd = vsoa_client_fds(client, &fds);
        
        cnt = pselect(max_fd + 1, &fds, NULL, NULL, &timeout, NULL);
        if (cnt > 0) {
            if (!vsoa_client_input_fds(client, &fds)) {
                fprintf(stderr, "Connection lost!\n");
                return -1;
            }
        }
        
        // Send gyro data every 10ms
        if (!send_gyro_data()) {
            fprintf(stderr, "Failed to send gyro data!\n");
        }
        
        // Send star data every 10ms
        if (!send_star_data()) {
            fprintf(stderr, "Failed to send star data!\n");
        }
        
        // Sleep for 10ms
        usleep(10000);
    }
    
    vsoa_client_close(client);
    return 0;
}
