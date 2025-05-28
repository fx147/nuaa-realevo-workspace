#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include "computility.h"  // 包含StarSensor和AttitudeData的定义

// 模拟传感器噪声
double add_noise(double value, double noise_level);

// 初始化陀螺仪数据
void init_gyro_data(FiberGyro *gyro1, MEMSGyro *gyro2);

// 初始化星敏感器数据
void init_star_sensor_data(StarSensor *star1, StarSensor *star2);

// 初始化姿态数据
void init_attitude_data(AttitudeData *data);

// 模拟休眠（跨平台兼容）
void msleep(unsigned int millis);

#endif // DATAPROCESSING_H
