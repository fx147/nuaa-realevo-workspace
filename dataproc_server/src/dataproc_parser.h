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

// �����ͻ��˴�����JSON���ݵ�Gyro1, Gyro2, OrbCtrlAllowFlag
bool parse_gyro_payload(const vsoa_payload_t *payload,
                      FiberGyro *Gyro1,
                      MEMSGyro *Gyro2,
                      int *OrbCtrlAllowFlag);

#endif // DATAPROC_PARSER_H
