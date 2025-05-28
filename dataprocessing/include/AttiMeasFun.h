/* ========================================================= */
/*  AttiMeasFun.c
 *
 * Last Edit: 2023年7月5日 9:30
 *
 * 姿态测量相关函数
 *
 * 星敏感器测量信息融合函数、测量信息坐标系转换函数
 * 滤波器量测更新和时间更新函数 */
/* ========================================================= */

#include <math.h>
#include <selfmath.h>

void qinv(double qout[4], double qin[4]);
void qleft(double Q1[4][4], double qinn[4]);
void CrossMatrixGet(double element1[3][3], double B[3]);
void StrGyroFltInit();
void StrGyroFltMeasUpd();
void StrGyroFltTimUpd(double Wbi_est[3], double Wbo_est[3]);
void qAverage(double q[4], double a[4], double b[4]);
void STmeas2body(double qbi_meas[4], double qsi_meas[4], double StarQuatInstall[4]);
void GYmeas2body(double wbi_meas[3], double wgi_meas[3], double GyroMtxInstall[3][3]);
void J2O(double I2O[3][3], double PVInJ[6]);
void A2q(double q[4], double A[3][3]);
void TransQ(double Q[3][3], double q[4]);
void qua2euler(double euler_angle[3], double q[4], double euler_angle_last[3]);
void QuatObit2Body(double qbo[4], double qoi[4], double qbi[4]);
void AngVelObit2Body(double wbo[3], double wbi[3], double OrbitVel, double qbo[4]);
void findMaxAbsoluteValue(double* maxValue, double Vector[3], int size);
void AttiResultValue(double AttiQuatEst[4], double AttiVelEst[3], double AttiEulerEst[3], double AttiEulerEstLast[3], double q[4], double w[3]);
void QuatNormalize(double Q_out[4], double Q_input[4]);
void QuatPositive(double Q_out[4], double Q_input[4]);
void GetDeltaQuat(double DeltaQuat[4], double Q1[4], double Q2[4]);
