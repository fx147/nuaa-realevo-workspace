/*
 * dataproc_parser.h
 *
 *  Created on: 2025��5��28��
 *      Author: fanxu
 */

#ifndef DATAPROC_PARSER_H
#define DATAPROC_PARSER_H

#include <stdbool.h>
#include "vsoa_parser.h"  // ����vsoa_payload_t�Ķ���
#include "computility.h"  // ����FiberGyro��MEMSGyro�Ķ���
#include "yyjson.h"      // ����yyjson��صĶ���

// ������������������
bool parse_fiber_gyro(yyjson_val *gyro_val, FiberGyro *gyro);

// ����MEMS����������
bool parse_mems_gyro(yyjson_val *gyro_val, MEMSGyro *gyro);

// 解析客户端传输的JSON数据的Gyro1, Gyro2, OrbCtrlAllowFlag, wbi_StarMeas
bool parse_gyro_payload(const vsoa_payload_t *payload,
                      FiberGyro *Gyro1,
                      MEMSGyro *Gyro2,
                      int *OrbCtrlAllowFlag,
                      double wbi_StarMeas[3]);

// Parse StarSensor structure from JSON
bool parse_star_sensor(yyjson_val *star_val, StarSensor *star);

// Parse AttitudeData structure from JSON
bool parse_attitude_data(yyjson_val *atti_val, AttitudeData *atti);

// Parse client JSON data for Star1, Star2, AttitudeData
bool parse_star_payload(const vsoa_payload_t *payload,
                       StarSensor *Star1,
                       StarSensor *Star2,
                       AttitudeData *AttiData);

#endif // DATAPROC_PARSER_H
