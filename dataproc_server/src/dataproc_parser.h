/*
 * dataproc_parser.h
 *
 *  Created on: 2025年5月28日
 *      Author: fanxu
 */

#ifndef DATAPROC_PARSER_H
#define DATAPROC_PARSER_H

#include <stdbool.h>
#include "vsoa_parser.h"  // 包含vsoa_payload_t的定义
#include "computility.h"  // 包含FiberGyro和MEMSGyro的定义
#include "yyjson.h"      // 包含yyjson相关的定义

// 解析光纤陀螺仪数据
bool parse_fiber_gyro(yyjson_val *gyro_val, FiberGyro *gyro);

// 解析MEMS陀螺仪数据
bool parse_mems_gyro(yyjson_val *gyro_val, MEMSGyro *gyro);

// 解析客户端传来的JSON数据到Gyro1, Gyro2, OrbCtrlAllowFlag
bool parse_gyro_payload(const vsoa_payload_t *payload,
                      FiberGyro *Gyro1,
                      MEMSGyro *Gyro2,
                      int *OrbCtrlAllowFlag);

#endif // DATAPROC_PARSER_H
