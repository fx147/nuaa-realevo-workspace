#ifndef DATAPROC_SERVER_H
#define DATAPROC_SERVER_H

#include "vsoa_server.h"    // 包含vsoa_server_t等的定义
#include "computility.h"    // 包含FiberGyro, MEMSGyro, StarSensor, AttitudeData的定义
#include "dataproc_parser.h" // 包含parse_gyro_payload函数的声明

// 从dataprocessing.h导入函数声明
#include "dataproc.h"

// 陀螺仪数据处理回调函数
static void command_gyro(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
                        vsoa_header_t *vsoa_hdr, vsoa_url_t *url,
                        vsoa_payload_t *payload);

// 星敏感器数据处理回调函数
static void command_star(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
                         vsoa_header_t *vsoa_hdr, vsoa_url_t *url,
                         vsoa_payload_t *payload);

#endif // DATAPROC_SERVER_H
