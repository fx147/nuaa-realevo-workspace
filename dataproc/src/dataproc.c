#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "computility.h"
#include "selfmath.h"
#include "AttiMeasFun.h"
#include "dataproc.h"

#define LOOP_INTERVAL_MS    10

double add_noise(double value, double noise_level) {
    return value + ((double)rand() / RAND_MAX - 0.5) * noise_level;
}

void init_gyro_data(FiberGyro *gyro1, MEMSGyro *gyro2) {
    gyro1->Flag = 1;
    gyro1->OnFlag = 1;
    gyro1->FaultCnt = 0;
    for (int i = 0; i < 3; ++i)
        gyro1->Meas[i] = add_noise(0.01 * i, 0.001);

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            gyro1->MtxInstall[i][j] = (i == j) ? 1.0 : 0.0;

    gyro2->Flag = 1;
    gyro2->OnFlag = 1;
    gyro2->FaultCnt = 0;
    for (int i = 0; i < 3; ++i)
        gyro2->Meas[i] = add_noise(0.02 * i, 0.001);

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            gyro2->MtxInstall[i][j] = (i == j) ? 1.0 : 0.0;
}


void init_star_sensor_data(StarSensor *star1, StarSensor *star2) {
    star1->Flag = 1;
    star1->OnFlag = 1;
    star1->FaultCnt = 0;
    star1->StarNum = 1;
    for (int i = 0; i < 4; ++i)
        star1->Meas[i] = (i == 0) ? 1.0 : add_noise(0.0, 0.001);
    for (int i = 0; i < 3; ++i)
        star1->MeasVel[i] = add_noise(0.01 * i, 0.0005);

    for (int i = 0; i < 4; ++i)
        star1->QuatInstall[i] = (i == 0) ? 1.0 : 0.0;

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            star1->MtxInstall[i][j] = (i == j) ? 1.0 : 0.0;

    star2->Flag = 1;
    star2->OnFlag = 1;
    star2->FaultCnt = 0;
    star2->StarNum = 2;
    for (int i = 0; i < 4; ++i)
        star2->Meas[i] = (i == 0) ? 1.0 : add_noise(0.0, 0.001);
    for (int i = 0; i < 3; ++i)
        star2->MeasVel[i] = add_noise(0.01 * i, 0.0005);

    for (int i = 0; i < 4; ++i)
        star2->QuatInstall[i] = (i == 0) ? 1.0 : 0.0;

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            star2->MtxInstall[i][j] = (i == j) ? 1.0 : 0.0;
}

void init_attitude_data(AttitudeData *data) {
    data->QuatOrbit[0] = 1.0;
    for (int i = 1; i < 4; ++i)
        data->QuatOrbit[i] = 0.0;

    data->AttiFiltValidFlag = 1;
    data->AttiQuatEst[0] = 1.0;
    for (int i = 1; i < 4; ++i)
        data->AttiQuatEst[i] = 0.0;

    data->OrbCtrlAllowFlag = 1;
}

void msleep(unsigned int millis) {
#ifdef _WIN32
    Sleep(millis);
#else
    usleep(millis * 1000);
#endif
}

