/*
 * dataproc_parser.c
 *
 *  Created on: 2025��5��28��
 *      Author: fanxu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "yyjson.h"
#include "vsoa_parser.h"
#include "computility.h"

bool parse_fiber_gyro(yyjson_val *gyro_val, FiberGyro *gyro) {
    if (!gyro_val || !yyjson_is_obj(gyro_val) || !gyro) {
        return false;
    }

    // ���� Flag
    yyjson_val *flag_val = yyjson_obj_get(gyro_val, "Flag");
    if (flag_val && yyjson_is_int(flag_val)) {
        gyro->Flag = (int)yyjson_get_sint(flag_val);
    }

    // ���� OnFlag
    yyjson_val *on_flag_val = yyjson_obj_get(gyro_val, "OnFlag");
    if (on_flag_val && yyjson_is_int(on_flag_val)) {
        gyro->OnFlag = (int)yyjson_get_sint(on_flag_val);
    }

    // ���� FaultCnt
    yyjson_val *fault_cnt_val = yyjson_obj_get(gyro_val, "FaultCnt");
    if (fault_cnt_val && yyjson_is_int(fault_cnt_val)) {
        gyro->FaultCnt = (int)yyjson_get_sint(fault_cnt_val);
    }

    // ���� Meas
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

    // ���� MtxInstall
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

// ���� MEMS ����������
bool parse_mems_gyro(yyjson_val *gyro_val, MEMSGyro *gyro) {
    if (!gyro_val || !yyjson_is_obj(gyro_val) || !gyro) {
        return false;
    }

    // ���� Flag
    yyjson_val *flag_val = yyjson_obj_get(gyro_val, "Flag");
    if (flag_val && yyjson_is_int(flag_val)) {
        gyro->Flag = (int)yyjson_get_sint(flag_val);
    }

    // ���� OnFlag
    yyjson_val *on_flag_val = yyjson_obj_get(gyro_val, "OnFlag");
    if (on_flag_val && yyjson_is_int(on_flag_val)) {
        gyro->OnFlag = (int)yyjson_get_sint(on_flag_val);
    }

    // ���� FaultCnt
    yyjson_val *fault_cnt_val = yyjson_obj_get(gyro_val, "FaultCnt");
    if (fault_cnt_val && yyjson_is_int(fault_cnt_val)) {
        gyro->FaultCnt = (int)yyjson_get_sint(fault_cnt_val);
    }

    // ���� Meas
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

    // ���� MtxInstall
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

// �����ͻ��˴����� JSON ���ݵ� Gyro1, Gyro2, OrbCtrlAllowFlag, wbi_StarMeas
bool parse_gyro_payload(const vsoa_payload_t *payload,
                        FiberGyro *Gyro1,
                        MEMSGyro *Gyro2,
                        int *OrbCtrlAllowFlag,
                        double wbi_StarMeas[3]) {
    if (!payload || !payload->param || !Gyro1 || !Gyro2 || !OrbCtrlAllowFlag || !wbi_StarMeas) {
        return false;
    }

    // ���� JSON
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

    // ���� Gyro1
    yyjson_val *gyro1_val = yyjson_obj_get(root, "Gyro1");
    if (!gyro1_val || !parse_fiber_gyro(gyro1_val, Gyro1)) {
        yyjson_doc_free(doc);
        return false;
    }

    // ���� Gyro2
    yyjson_val *gyro2_val = yyjson_obj_get(root, "Gyro2");
    if (!gyro2_val || !parse_mems_gyro(gyro2_val, Gyro2)) {
        yyjson_doc_free(doc);
        return false;
    }

    // ���� OrbCtrlAllowFlag
    yyjson_val *flag_val = yyjson_obj_get(root, "OrbCtrlAllowFlag");
    if (!flag_val || !yyjson_is_int(flag_val)) {
        yyjson_doc_free(doc);
        return false;
    }
    *OrbCtrlAllowFlag = (int)yyjson_get_sint(flag_val);

    // ���� wbi_StarMeas
    yyjson_val *wbi_star_val = yyjson_obj_get(root, "wbi_StarMeas");
    if (wbi_star_val && yyjson_is_arr(wbi_star_val)) {
        yyjson_val *meas_item;
        size_t idx, max;
        yyjson_arr_foreach(wbi_star_val, idx, max, meas_item) {
            if (yyjson_is_real(meas_item) && idx < 3) {
                wbi_StarMeas[idx] = yyjson_get_real(meas_item);
            }
        }
    } else {
        // ���û���ṩ wbi_StarMeas������Ϊĭ��ֵ 0
        wbi_StarMeas[0] = 0.0;
        wbi_StarMeas[1] = 0.0;
        wbi_StarMeas[2] = 0.0;
    }

    yyjson_doc_free(doc);
    return true;
}

// Parse StarSensor structure from JSON
bool parse_star_sensor(yyjson_val *star_val, StarSensor *star) {
    if (!star_val || !yyjson_is_obj(star_val) || !star) {
        return false;
    }

    // Parse Flag
    yyjson_val *flag_val = yyjson_obj_get(star_val, "Flag");
    if (flag_val && yyjson_is_int(flag_val)) {
        star->Flag = (int)yyjson_get_sint(flag_val);
    }

    // Parse OnFlag
    yyjson_val *on_flag_val = yyjson_obj_get(star_val, "OnFlag");
    if (on_flag_val && yyjson_is_int(on_flag_val)) {
        star->OnFlag = (int)yyjson_get_sint(on_flag_val);
    }

    // Parse FaultCnt
    yyjson_val *fault_cnt_val = yyjson_obj_get(star_val, "FaultCnt");
    if (fault_cnt_val && yyjson_is_int(fault_cnt_val)) {
        star->FaultCnt = (int)yyjson_get_sint(fault_cnt_val);
    }

    // Parse StarNum
    yyjson_val *star_num_val = yyjson_obj_get(star_val, "StarNum");
    if (star_num_val && yyjson_is_int(star_num_val)) {
        star->StarNum = (int)yyjson_get_sint(star_num_val);
    }

    // Parse Meas (4 elements)
    yyjson_val *meas_val = yyjson_obj_get(star_val, "Meas");
    if (meas_val && yyjson_is_arr(meas_val)) {
        yyjson_val *meas_item;
        size_t idx, max;
        yyjson_arr_foreach(meas_val, idx, max, meas_item) {
            if (yyjson_is_real(meas_item)) {
                if (idx >= 0 && idx < 4) {
                    star->Meas[idx] = yyjson_get_real(meas_item);
                }
            }
        }
    }

    // Parse MeasVel (3 elements)
    yyjson_val *meas_vel_val = yyjson_obj_get(star_val, "MeasVel");
    if (meas_vel_val && yyjson_is_arr(meas_vel_val)) {
        yyjson_val *meas_vel_item;
        size_t idx, max;
        yyjson_arr_foreach(meas_vel_val, idx, max, meas_vel_item) {
            if (yyjson_is_real(meas_vel_item)) {
                if (idx >= 0 && idx < 3) {
                    star->MeasVel[idx] = yyjson_get_real(meas_vel_item);
                }
            }
        }
    }

    // Parse QuatInstall (4 elements)
    yyjson_val *quat_install_val = yyjson_obj_get(star_val, "QuatInstall");
    if (quat_install_val && yyjson_is_arr(quat_install_val)) {
        yyjson_val *quat_item;
        size_t idx, max;
        yyjson_arr_foreach(quat_install_val, idx, max, quat_item) {
            if (yyjson_is_real(quat_item)) {
                if (idx >= 0 && idx < 4) {
                    star->QuatInstall[idx] = yyjson_get_real(quat_item);
                }
            }
        }
    }

    // Parse MtxInstall (3x3 matrix)
    yyjson_val *mtx_install_val = yyjson_obj_get(star_val, "MtxInstall");
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
                        star->MtxInstall[row_index][col_index] = yyjson_get_real(col_val);
                    }
                    col_index++;
                }
            }
            row_index++;
        }
    }

    return true;
}

// Parse AttitudeData structure from JSON
bool parse_attitude_data(yyjson_val *atti_val, AttitudeData *atti) {
    if (!atti_val || !yyjson_is_obj(atti_val) || !atti) {
        return false;
    }

    // Parse AttiFiltValidFlag
    yyjson_val *filt_flag_val = yyjson_obj_get(atti_val, "AttiFiltValidFlag");
    if (filt_flag_val && yyjson_is_int(filt_flag_val)) {
        atti->AttiFiltValidFlag = (int)yyjson_get_sint(filt_flag_val);
    }

    // Parse OrbCtrlAllowFlag
    yyjson_val *orb_flag_val = yyjson_obj_get(atti_val, "OrbCtrlAllowFlag");
    if (orb_flag_val && yyjson_is_int(orb_flag_val)) {
        atti->OrbCtrlAllowFlag = (int)yyjson_get_sint(orb_flag_val);
    }

    // Parse QuatOrbit (4 elements)
    yyjson_val *quat_orbit_val = yyjson_obj_get(atti_val, "QuatOrbit");
    if (quat_orbit_val && yyjson_is_arr(quat_orbit_val)) {
        yyjson_val *quat_item;
        size_t idx, max;
        yyjson_arr_foreach(quat_orbit_val, idx, max, quat_item) {
            if (yyjson_is_real(quat_item)) {
                if (idx >= 0 && idx < 4) {
                    atti->QuatOrbit[idx] = yyjson_get_real(quat_item);
                }
            }
        }
    }

    // Parse AttiQuatEst (4 elements)
    yyjson_val *quat_est_val = yyjson_obj_get(atti_val, "AttiQuatEst");
    if (quat_est_val && yyjson_is_arr(quat_est_val)) {
        yyjson_val *quat_item;
        size_t idx, max;
        yyjson_arr_foreach(quat_est_val, idx, max, quat_item) {
            if (yyjson_is_real(quat_item)) {
                if (idx >= 0 && idx < 4) {
                    atti->AttiQuatEst[idx] = yyjson_get_real(quat_item);
                }
            }
        }
    }

    // Parse QuatAverage (4 elements)
    yyjson_val *quat_avg_val = yyjson_obj_get(atti_val, "QuatAverage");
    if (quat_avg_val && yyjson_is_arr(quat_avg_val)) {
        yyjson_val *quat_item;
        size_t idx, max;
        yyjson_arr_foreach(quat_avg_val, idx, max, quat_item) {
            if (yyjson_is_real(quat_item)) {
                if (idx >= 0 && idx < 4) {
                    atti->QuatAverage[idx] = yyjson_get_real(quat_item);
                }
            }
        }
    }

    // Parse wbi_StarMeas (3 elements)
    yyjson_val *wbi_star_val = yyjson_obj_get(atti_val, "wbi_StarMeas");
    if (wbi_star_val && yyjson_is_arr(wbi_star_val)) {
        yyjson_val *wbi_item;
        size_t idx, max;
        yyjson_arr_foreach(wbi_star_val, idx, max, wbi_item) {
            if (yyjson_is_real(wbi_item)) {
                if (idx >= 0 && idx < 3) {
                    atti->wbi_StarMeas[idx] = yyjson_get_real(wbi_item);
                }
            }
        }
    }

    return true;
}

// Parse client JSON data for Star1, Star2, AttitudeData
bool parse_star_payload(const vsoa_payload_t *payload,
                       StarSensor *Star1,
                       StarSensor *Star2,
                       AttitudeData *AttiData) {
    if (!payload || !payload->param || !Star1 || !Star2 || !AttiData) {
        return false;
    }

    // Parse JSON
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

    // Parse Star1
    yyjson_val *star1_val = yyjson_obj_get(root, "Star1");
    if (!star1_val || !parse_star_sensor(star1_val, Star1)) {
        yyjson_doc_free(doc);
        return false;
    }

    // Parse Star2
    yyjson_val *star2_val = yyjson_obj_get(root, "Star2");
    if (!star2_val || !parse_star_sensor(star2_val, Star2)) {
        yyjson_doc_free(doc);
        return false;
    }

    // Parse AttitudeData
    yyjson_val *atti_val = yyjson_obj_get(root, "AttitudeData");
    if (!atti_val || !parse_attitude_data(atti_val, AttiData)) {
        yyjson_doc_free(doc);
        return false;
    }

    yyjson_doc_free(doc);
    return true;
}
