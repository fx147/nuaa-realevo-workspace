#ifndef COMPUTILITY_H
#define COMPUTILITY_H

// 定义光纤陀螺结构体
typedef struct {
    int Flag;                               // 光纤陀螺仪有效标志位
    double Meas[3];                         // 光纤陀螺仪测量值
    double MtxInstall[3][3];                // 光纤陀螺仪安装矩阵（实际到本体）
    int FaultCnt;                           // 故障计数
    int OnFlag;                             // 开启标志位
} FiberGyro;

// 定义微机械惯组结构体
typedef struct {
    int Flag;                               // 微机械惯组有效标志位
    double Meas[3];                         // 微机械惯组测量值
    double MtxInstall[3][3];                // 微机械惯组安装矩阵（实际到本体）
    int FaultCnt;                           // 故障计数
    int OnFlag;                             // 开启标志位
} MEMSGyro;

// 定义星敏感器结构体
typedef struct {
    int Flag;                               // 星敏感器有效标志位
    double Meas[4];                         // 星敏感器测量值
    double MeasVel[3];                      // 星敏感器角速度测量值
    double QuatInstall[4];                  // 星敏感器安装矩阵（实际到本体）
    double MtxInstall[3][3];                // 星敏感器安装矩阵（实际到本体）
    int StarNum;                            // 星敏感器编号
    int FaultCnt;                           // 故障计数
    int OnFlag;                             // 开启标志位
} StarSensor;

// 定义姿态数据结构体
typedef struct {
    double QuatOrbit[4];                    // 惯性系相对于轨道系四元数
    int AttiFiltValidFlag;                  // 滤波器收敛标志位
    double AttiQuatEst[4];                  // 滤波器估计四元数
    double QuatAverage[4];                  // 四元数平均值
    double wbi_StarMeas[3];                 // 星敏感器角速度测量值
    int OrbCtrlAllowFlag;                   // 轨控允许标志位
} AttitudeData;

void GyroDataProcessing(FiberGyro Gyro1, MEMSGyro Gyro2, int OrbCtrlAllowFlag, double wbi_StarMeas[3]);
void StarDataProcessing(StarSensor Star1,StarSensor Star2, AttitudeData AttiData);
#endif // COMPUTILITY_H