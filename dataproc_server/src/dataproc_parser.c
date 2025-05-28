/*
 * dataproc_parser.c
 *
 *  Created on: 2025年5月28日
 *      Author: fanxu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "yyjson.h"
#include "vsoa_parser.h"
#include "computility.h"

// 解析光纤陀螺仪数据
bool parse_fiber_gyro(yyjson_val *gyro_val, FiberGyro *gyro) {
    if (!gyro_val || !yyjson_is_obj(gyro_val) || !gyro) {
        return false;
    }

    // 解析 Flag
    yyjson_val *flag_val = yyjson_obj_get(gyro_val, "Flag");
    if (flag_val && yyjson_is_int(flag_val)) {
        gyro->Flag = (int)yyjson_get_sint(flag_val);
    }

    // 解析 OnFlag
    yyjson_val *on_flag_val = yyjson_obj_get(gyro_val, "OnFlag");
    if (on_flag_val && yyjson_is_int(on_flag_val)) {
        gyro->OnFlag = (int)yyjson_get_sint(on_flag_val);
    }

    // 解析 FaultCnt
    yyjson_val *fault_cnt_val = yyjson_obj_get(gyro_val, "FaultCnt");
    if (fault_cnt_val && yyjson_is_int(fault_cnt_val)) {
        gyro->FaultCnt = (int)yyjson_get_sint(fault_cnt_val);
    }

    // 解析 Meas
    yyjson_val *meas_val = yyjson_obj_get(gyro_val, "Meas");
    if (meas_val && yyjson_is_arr(meas_val)) {
        yyjson_val *meas_item;
        size_t idx, max;
        yyjson_arr_foreach(meas_val, idx, max, meas_item) {
            if (yyjson_is_real(meas_item)) {
                if (idx >= 0 && idx < 3) {
                    gyro->Meas[idx] = yyjson_get_real(meas_item);
                }
            }
        }
    }

    // 解析 MtxInstall
    yyjson_val *mtx_install_val = yyjson_obj_get(gyro_val, "MtxInstall");
    if (mtx_install_val && yyjson_is_arr(mtx_install_val)) {
        yyjson_val *row_val;
        size_t row_idx, row_max;
        int row_index = 0;
        yyjson_arr_foreach(mtx_install_val, row_idx, row_max, row_val) {
            if (yyjson_is_arr(row_val)) {
                yyjson_val *col_val;
                size_t col_idx, col_max;
                int col_index = 0;
                yyjson_arr_foreach(row_val, col_idx, col_max, col_val) {
                    if (yyjson_is_real(col_val) && row_index < 3 && col_index < 3) {
                        gyro->MtxInstall[row_index][col_index] = yyjson_get_real(col_val);
                    }
                    col_index++;
                }
            }
            row_index++;
        }
    }

    return true;
}

// 解析 MEMS 陀螺仪数据
bool parse_mems_gyro(yyjson_val *gyro_val, MEMSGyro *gyro) {
    if (!gyro_val || !yyjson_is_obj(gyro_val) || !gyro) {
        return false;
    }

    // 解析 Flag
    yyjson_val *flag_val = yyjson_obj_get(gyro_val, "Flag");
    if (flag_val && yyjson_is_int(flag_val)) {
        gyro->Flag = (int)yyjson_get_sint(flag_val);
    }

    // 解析 OnFlag
    yyjson_val *on_flag_val = yyjson_obj_get(gyro_val, "OnFlag");
    if (on_flag_val && yyjson_is_int(on_flag_val)) {
        gyro->OnFlag = (int)yyjson_get_sint(on_flag_val);
    }

    // 解析 FaultCnt
    yyjson_val *fault_cnt_val = yyjson_obj_get(gyro_val, "FaultCnt");
    if (fault_cnt_val && yyjson_is_int(fault_cnt_val)) {
        gyro->FaultCnt = (int)yyjson_get_sint(fault_cnt_val);
    }

    // 解析 Meas
    yyjson_val *meas_val = yyjson_obj_get(gyro_val, "Meas");
    if (meas_val && yyjson_is_arr(meas_val)) {
        yyjson_val *meas_item;
        size_t idx, max;
        yyjson_arr_foreach(meas_val, idx, max, meas_item) {
            if (yyjson_is_real(meas_item)) {
                if (idx >= 0 && idx < 3) {
                    gyro->Meas[idx] = yyjson_get_real(meas_item);
                }
            }
        }
    }

    // 解析 MtxInstall
    yyjson_val *mtx_install_val = yyjson_obj_get(gyro_val, "MtxInstall");
    if (mtx_install_val && yyjson_is_arr(mtx_install_val)) {
        yyjson_val *row_val;
        size_t row_idx, row_max;
        int row_index = 0;
        yyjson_arr_foreach(mtx_install_val, row_idx, row_max, row_val) {
            if (yyjson_is_arr(row_val)) {
                yyjson_val *col_val;
                size_t col_idx, col_max;
                int col_index = 0;
                yyjson_arr_foreach(row_val, col_idx, col_max, col_val) {
                    if (yyjson_is_real(col_val) && row_index < 3 && col_index < 3) {
                        gyro->MtxInstall[row_index][col_index] = yyjson_get_real(col_val);
                    }
                    col_index++;
                }
            }
            row_index++;
        }
    }

    return true;
}

// 解析客户端传来的 JSON 数据到 Gyro1, Gyro2, OrbCtrlAllowFlag
bool parse_gyro_payload(const vsoa_payload_t *payload,
                        FiberGyro *Gyro1,
                        MEMSGyro *Gyro2,
                        int *OrbCtrlAllowFlag) {
    if (!payload || !payload->param || !Gyro1 || !Gyro2 || !OrbCtrlAllowFlag) {
        return false;
    }

    // 解析 JSON
    yyjson_doc *doc = yyjson_read(payload->param, payload->param_len, 0);
    if (!doc) {
        fprintf(stderr, "Failed to parse JSON\n");
        return false;
    }

    yyjson_val *root = yyjson_doc_get_root(doc);
    if (!yyjson_is_obj(root)) {
        yyjson_doc_free(doc);
        return false;
    }

    // 解析 Gyro1
    yyjson_val *gyro1_val = yyjson_obj_get(root, "Gyro1");
    if (!gyro1_val || !parse_fiber_gyro(gyro1_val, Gyro1)) {
        yyjson_doc_free(doc);
        return false;
    }

    // 解析 Gyro2
    yyjson_val *gyro2_val = yyjson_obj_get(root, "Gyro2");
    if (!gyro2_val || !parse_mems_gyro(gyro2_val, Gyro2)) {
        yyjson_doc_free(doc);
        return false;
    }

    // 解析 OrbCtrlAllowFlag
    yyjson_val *flag_val = yyjson_obj_get(root, "OrbCtrlAllowFlag");
    if (!flag_val || !yyjson_is_int(flag_val)) {
        yyjson_doc_free(doc);
        return false;
    }
    *OrbCtrlAllowFlag = (int)yyjson_get_sint(flag_val);

    yyjson_doc_free(doc);
    return true;
}
