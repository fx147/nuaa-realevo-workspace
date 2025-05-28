#ifndef LIBDATAPROC_DATAPROC_H_
#define LIBDATAPROC_DATAPROC_H_

#include "computility.h"  // 已包含结构体定义 FiberGyro, MEMSGyro, StarSensor, AttitudeData
#include <math.h>

// 函数声明

/**
 * @brief 添加高斯噪声到输入值
 * @param value 原始值
 * @param noise_level 噪声幅值（±noise_level/2）
 * @return double 含噪声的输出值
 */
double add_noise(double value, double noise_level);

/**
 * @brief 初始化陀螺仪数据
 * @param gyro1 光纤陀螺指针
 * @param gyro2 微机械陀螺指针
 */
void init_gyro_data(FiberGyro *gyro1, MEMSGyro *gyro2);

/**
 * @brief 初始化星敏感器数据
 * @param star1 星敏感器1指针
 * @param star2 星敏感器2指针
 */
void init_star_sensor_data(StarSensor *star1, StarSensor *star2);

/**
 * @brief 初始化姿态数据
 * @param data 姿态数据结构体指针
 */
void init_attitude_data(AttitudeData *data);

/**
 * @brief 跨平台毫秒级休眠函数
 * @param millis 休眠时间（毫秒）
 */
void msleep(unsigned int millis);

#endif /* LIBDATAPROC_DATAPROC_H_ */