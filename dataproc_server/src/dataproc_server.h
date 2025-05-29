#ifndef DATAPROC_SERVER_H
#define DATAPROC_SERVER_H

#include "vsoa_server.h"    // ����vsoa_server_t�ȵĶ���
#include "computility.h"    // ����FiberGyro, MEMSGyro, StarSensor, AttitudeData�Ķ���
#include "dataproc_parser.h" // ����parse_gyro_payload����������

// ��dataprocessing.h���뺯������
#include "dataproc.h"

// ���������ݴ���ص�����
static void command_gyro(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
                        vsoa_header_t *vsoa_hdr, vsoa_url_t *url,
                        vsoa_payload_t *payload);

// �����������ݴ���ص�����
static void command_star(void *arg, vsoa_server_t *server, vsoa_cli_id_t cid,
                         vsoa_header_t *vsoa_hdr, vsoa_url_t *url,
                         vsoa_payload_t *payload);

#endif // DATAPROC_SERVER_H
