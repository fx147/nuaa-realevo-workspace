/* ========================================================= */
/*  AttiMeasFun.c
 *
 * Last Edit: 2023年8月27日 22:15
 *
 * 姿态测量相关函数
 * 
 * 星敏感器测量信息融合函数、测量信息坐标系转换函数
 * 滤波器量测更新和时间更新函数 */
/* ========================================================= */

#include <stdio.h>
#include <stdlib.h>
#include "AttiMeasFun.h"
//#include <GNC_Init.h>


/* 四元数的逆 */
void qinv(double qout[4], double qin[4]) {

	double QuatNormSquared;

	QuatNormSquared = qin[0]*qin[0] + qin[1]*qin[1] + qin[2]*qin[2] + qin[3]*qin[3];
	qout[0] = qin[0] / QuatNormSquared;
	qout[1] = -qin[1] / QuatNormSquared;
	qout[2] = -qin[2] / QuatNormSquared;
	qout[3] = -qin[3] / QuatNormSquared;

	return;
}



/* 四元数左乘矩阵 */
void qleft(double Q1[4][4], double qinn[4]) {
	Q1[0][0] = qinn[0]; Q1[0][1] = -qinn[1]; Q1[0][2] = -qinn[2]; Q1[0][3] = -qinn[3];
	Q1[1][0] = qinn[1]; Q1[1][1] = qinn[0]; Q1[1][2] = -qinn[3]; Q1[1][3] = qinn[2];
	Q1[2][0] = qinn[2]; Q1[2][1] = qinn[3]; Q1[2][2] = qinn[0]; Q1[2][3] = -qinn[1];
	Q1[3][0] = qinn[3]; Q1[3][1] = -qinn[2]; Q1[3][2] = qinn[1]; Q1[3][3] = qinn[0];

	return;
}



/* 求取叉乘矩阵[Bx] */
void CrossMatrixGet(double element1[3][3], double B[3]) {

	element1[0][0] = 0; element1[0][1] = -B[2]; element1[0][2] = B[1];
	element1[1][0] = B[2]; element1[1][1] = 0; element1[1][2] = -B[0];
	element1[2][0] = -B[1]; element1[2][1] = B[0]; element1[2][2] = 0;

	return;
}



/* 双星敏陀螺ESKF初始化模块 */
// void StrGyroFltInit() {

// 	/* 盛一鸣，2023.06.20 */
// 	/* 滤波器开启或重启触发的初始化模块 */
// 	/* 输入量 */
// 	/* StrGyroFltQ   输入给滤波器的初始四元数 */
// 	/* StrGyroFltB   输入给滤波器的初始陀螺漂移 */
// 	/* StrGyroFltP   输入给滤波器的初始估计均方误差阵 */
// 	/* QuatOrbit     惯性相对于轨道的四元数 */
// 	/* qbi_meas      星敏感器测量值 */
// 	/* 输出量 */
// 	/* StrGyroFltQ   四元数初始估计值 */
// 	/* StrGyroFltB   陀螺漂移初始估计值 */
// 	/* StrGyroFltP   初始估计均方误差阵 */

// 	static double tmp, qoi_n[4], Q11[4][4];

// 	if (StarValidFlag == 1){
// 		qinv(qoi_n, QuatOrbit);
// 		qleft(Q11, qoi_n);
// 		mtxMtp((double*)StrGyroFltQ, (double*)Q11, 4, 4, (double*)qbi_meas, 4, 1);
// 		tmp = -1.0;
// 		if (StrGyroFltQ[0] < 0) {
// 			mtxMtp((double*)StrGyroFltQ, (double*)StrGyroFltQ, 4, 1, &tmp, 1, 1);
// 		}

// 		tmp = 0.01;
// 		StrGyroFltP[0][0] = tmp; StrGyroFltP[0][1] = 0; StrGyroFltP[0][2] = 0; StrGyroFltP[0][3] = 0; StrGyroFltP[0][4] = 0; StrGyroFltP[0][5] = 0;
// 		StrGyroFltP[1][0] = 0; StrGyroFltP[1][1] = tmp; StrGyroFltP[1][2] = 0; StrGyroFltP[1][3] = 0; StrGyroFltP[1][4] = 0; StrGyroFltP[1][5] = 0;
// 		StrGyroFltP[2][0] = 0; StrGyroFltP[2][1] = 0; StrGyroFltP[2][2] = tmp; StrGyroFltP[2][3] = 0; StrGyroFltP[2][4] = 0; StrGyroFltP[2][5] = 0;
// 		StrGyroFltP[3][0] = 0; StrGyroFltP[3][1] = 0; StrGyroFltP[3][2] = 0; StrGyroFltP[3][3] = tmp; StrGyroFltP[3][4] = 0; StrGyroFltP[3][5] = 0;
// 		StrGyroFltP[4][0] = 0; StrGyroFltP[4][1] = 0; StrGyroFltP[4][2] = 0; StrGyroFltP[4][3] = 0; StrGyroFltP[4][4] = tmp; StrGyroFltP[4][5] = 0;
// 		StrGyroFltP[5][0] = 0; StrGyroFltP[5][1] = 0; StrGyroFltP[5][2] = 0; StrGyroFltP[5][3] = 0; StrGyroFltP[5][4] = 0; StrGyroFltP[5][5] = tmp;

// 		StrGyroFltB[0] = 0;
// 		StrGyroFltB[1] = 0;
// 		StrGyroFltB[2] = 0;
// 	}

// }



/* 双星敏陀螺ESKF的量测更新 */
// void StrGyroFltMeasUpd() {

// 	/* 盛一鸣，2023.06.20 */
// 	/* 滤波器量测更新模块 */
// 	/* 输入量 */
// 	/* StrGyroFltQ   输入给滤波器的四元数预测值 */
// 	/* StrGyroFltB   输入给滤波器的陀螺漂移预测值 */
// 	/* StrGyroFltP   输入给滤波器的预测均方误差阵 */
// 	/* QuatOrbit     惯性相对于轨道的四元数 */
// 	/* qbi_meas      星敏感器测量值 */
// 	/* GyroStarCnt   滤波器计数器 */
// 	/* 输出量 */
// 	/* StrGyroFltQ   四元数估计值 */
// 	/* StrGyroFltB   陀螺漂移估计值 */
// 	/* StrGyroFltP   估计均方误差阵 */

// 	static double JacobianH[3][6], matrixR[3][3], tmp1[3][6], JacobianH_T[6][3], tmp2[3][3], tmp3[3][3], tmp4[3][3], tmp5[6][3], K[6][3], Q11[4][4];
// 	static double qbi_pre[4], qbi_pre_n[4], Q111[4][4], err_tmp[4], err[3], StateDelta[6], QDelta[3], QDeltaScalar, BDelta[3], Q2[4], Q3[4][4], ii, jj, tmp1_8[6][6];
// 	static int i, j;
// 	static double tmp1_9[6][6], tmp1_10[6][6], tmp1_11[6][6], tmp1_12[6][3], tmp1_13[3][6], tmp1_14[6][6];


// 	/* 求取H矩阵 */
// 	for (i = 0; i < 3; i++) {
// 		for (j = 0; j < 6; j++) {
// 			JacobianH[i][j] = 0.0;
// 		}
// 	}
// 	JacobianH[0][3] = 1.0; JacobianH[1][4] = 1.0; JacobianH[2][5] = 1.0;

// 	/* 求取R矩阵 */
// 	matrixR[0][0] = StrGyroFltNoiseR1; matrixR[0][1] = 0; matrixR[0][2] = 0;
// 	matrixR[1][0] = 0; matrixR[1][1] = StrGyroFltNoiseR2; matrixR[1][2] = 0;
// 	matrixR[2][0] = 0; matrixR[2][1] = 0; matrixR[2][2] = StrGyroFltNoiseR3;

// 	/* 求取K矩阵 */
// 	mtxMtp((double*)tmp1, (double*)JacobianH, 3, 6, (double*)StrGyroFltP, 6, 6);
// 	mtxT((double*)JacobianH_T, (double*)JacobianH, 6, 3);
// 	mtxMtp((double*)tmp2, (double*)tmp1, 3, 6, (double*)JacobianH_T, 6, 3);
// 	mtxAdd((double*)tmp3, (double*)tmp2, (double*)matrixR, 3, 3);
// 	mtxInv((double*)tmp4, (double*)tmp3, 3);
// 	mtxMtp((double*)tmp5, (double*)StrGyroFltP, 6, 6, (double*)JacobianH_T, 6, 3);
// 	mtxMtp((double*)K, (double*)tmp5, 6, 3, (double*)tmp4, 3, 3);

// 	/* 求取状态偏差量 */
// 	qleft(Q11, QuatOrbit);
// 	mtxMtp((double*)qbi_pre, (double*)Q11, 4, 4, (double*)StrGyroFltQ, 4, 1);
// 	ii = -1.0;
// 	if (qbi_pre[0] < 0) {
// 		mtxMtp((double*)qbi_pre, (double*)qbi_pre, 4, 1, &ii, 1, 1);
// 	}

// 	qinv(qbi_pre_n, qbi_pre);
// 	qleft(Q111, qbi_pre_n);
// 	mtxMtp((double*)err_tmp, (double*)Q111, 4, 4, (double*)qbi_meas, 4, 1);
// 	ii = -1.0;
// 	if (err_tmp[0] < 0) {
// 		mtxMtp((double*)err_tmp, (double*)err_tmp, 4, 1, &ii, 1, 1);
// 	}

// 	err[0] = err_tmp[1]; err[1] = err_tmp[2]; err[2] = err_tmp[3];
// 	mtxMtp((double*)StateDelta, (double*)K, 6, 3, (double*)err, 3, 1);

// 	/* 进行状态更新delta_b=delta(1:3);delta_q=delta(4:6); */
// 	BDelta[0] = StateDelta[0];
// 	BDelta[1] = StateDelta[1];
// 	BDelta[2] = StateDelta[2];
// 	QDelta[0] = StateDelta[3];
// 	QDelta[1] = StateDelta[4];
// 	QDelta[2] = StateDelta[5];
// 	QDeltaScalar = sqrt(1 - norm(QDelta, 3) * norm(QDelta, 3));

// 	/*计算GyroStarqbo_est*/
// 	Q2[0] = QDeltaScalar;
// 	Q2[1] = QDelta[0];
// 	Q2[2] = QDelta[1];
// 	Q2[3] = QDelta[2];

// 	jj = norm(Q2,4); ii = 1.0 / jj;
// 	if (jj > 1) {
// 		mtxMtp((double*)Q2, (double*)Q2, 4, 1, &ii, 1, 1);
// 	}

// 	qleft(Q3, StrGyroFltQ);
// 	mtxMtp((double*)StrGyroFltQ, (double*)Q3, 4, 4, (double*)Q2, 4, 1);

// 	jj = norm(StrGyroFltQ,4); ii = 1.0 / jj;
// 	if (jj > 1) {
// 		mtxMtp((double*)StrGyroFltQ, (double*)StrGyroFltQ, 4, 1, &ii, 1, 1);
// 	}
// 	ii = -1.0;
// 	if (StrGyroFltQ[0] < 0) {
// 		mtxMtp((double*)StrGyroFltQ, (double*)StrGyroFltQ, 4, 1, &ii, 1, 1);
// 	}

// 	/*计算GyroStarb_est*/
// 	StrGyroFltB[0] += BDelta[0];
// 	StrGyroFltB[1] += BDelta[1];
// 	StrGyroFltB[2] += BDelta[2];

// 	ii = -1.0;
// 	mtxMtp((double*)tmp1_8, (double*)K, 6, 3, (double*)JacobianH, 3, 6);
// 	mtxMtp((double*)tmp1_8, (double*)tmp1_8, 6, 6, &ii, 1, 1);
// 	for (i = 0; i < 6; i++) {
// 		tmp1_8[i][i] += 1.0;
// 	}
// 	mtxMtp((double*)tmp1_9, (double*)tmp1_8, 6, 6, (double*)StrGyroFltP, 6, 6);
// 	mtxT((double*)tmp1_10, (double*)tmp1_8, 6, 6);
// 	mtxMtp((double*)tmp1_11, (double*)tmp1_9, 6, 6, (double*)tmp1_10, 6, 6);
// 	mtxMtp((double*)tmp1_12, (double*)K, 6, 3, (double*)matrixR, 3, 3);
// 	mtxT((double*)tmp1_13, (double*)K, 3, 6);
// 	mtxMtp((double*)tmp1_14, (double*)tmp1_12, 6, 3, (double*)tmp1_13, 3, 6);
// 	mtxAdd((double*)StrGyroFltP, (double*)tmp1_11, (double*)tmp1_14, 6, 6);
// }



/* 双星敏陀螺ESKF的时间更新 */
// void StrGyroFltTimUpd(double Wbi_est[3], double Wbo_est[3]) {

// 	/* 盛一鸣，2023.06.20 */
// 	/* 滤波器时间更新模块 */
// 	/* 输入量 */
// 	/* StrGyroFltQ   输入给滤波器的四元数估计值 */
// 	/* StrGyroFltB   输入给滤波器的陀螺漂移估计值 */
// 	/* StrGyroFltP   输入给滤波器的估计均方误差阵 */
// 	/* Wbi_est       惯性系相对于本体系的角速度估计值 */
// 	/* Wbo_est       轨道系相对于本体系的角速度估计值 */
// 	/* 输出量 */
// 	/* StrGyroFltQ   四元数预测值 */
// 	/* StrGyroFltB   陀螺漂移预测值 */
// 	/* StrGyroFltP   预测均方误差阵 */

// 	static double Estimation_W[3], tmp1[3][3], JacobianF[6][6], MatrixG[6][6], MatrixQ[6][6], tmp2[6][6], JacobianF_tmp[6][6], Fmatrix2[6][6], Fmatrix3[6][6];
// 	static double tmp3[6][6], tInterval3, tmp4[6][6], tmp5[6][6], Phi[6][6], tmp6[6][6], tInterval4, tmp7[6][6], ii, tmp8[6][6], tmp9[6][6], tmp10[6][6], AttMatrixQ[3][3];
// 	static int i, j;
// 	static double Toi[6][6], Phi_t[6][6], Toi_t[6][6], tmp11[6][6], Px[6][6], tmp12[6][6], tmp13[6][6], Rc, Wo, Woi[3], tmp14[3], q_pre[4], tInterval2;
// 	static double MatrixF[4][4], theta1[4][4], FltQInc[4];
// 	static double /*upStrGyroFltNoiseQ1, upStrGyroFltNoiseQ2,*/ tInterval, orbInfo[1], GM, pufaiX[3][3], Matrixtheta[3][3], fai[3][3], omega[4][4];


// 	//upStrGyroFltNoiseQ1 = 1E-10;
// 	//upStrGyroFltNoiseQ2 = 1E-10;
// 	tInterval = 1;


// 	/* 求取F矩阵 */
// 	CrossMatrixGet(tmp1, Wbi_est);

// 	for (i = 0; i < 3; i++) {
// 		for (j = 0; j < 6; j++) {
// 			JacobianF[i][j] = 0.0;
// 		}
// 	}
// 	JacobianF[3][0] = -0.5; JacobianF[3][1] = 0; JacobianF[3][2] = 0;
// 	JacobianF[4][0] = 0; JacobianF[4][1] = -0.5; JacobianF[4][2] = 0;
// 	JacobianF[5][0] = 0; JacobianF[5][1] = 0; JacobianF[5][2] = -0.5;
// 	for (i = 3; i < 6; i++) {
// 		for (j = 3; j < 6; j++) {
// 			JacobianF[i][j] = -1.0 * tmp1[i - 3][j - 3];
// 		}
// 	}

// 	/* 求取G、Q矩阵 */
// 	for (i = 0; i < 6; i++) {
// 		for (j = 0; j < 6; j++) {
// 			MatrixG[i][j] = 0.0;
// 			MatrixQ[i][j] = 0.0;
// 		}
// 	}
// 	/* 求取G矩阵 G=[eye(3,3) zeros(3,3);zeros(3,3) -0.5*eye(3,3)] */
// 	MatrixG[0][0] = 1.0; MatrixG[1][1] = 1.0; MatrixG[2][2] = 1.0;
// 	MatrixG[3][3] = -0.5; MatrixG[4][4] = -0.5; MatrixG[5][5] = -0.5;

// 	/* 求取Q矩阵 Q = [BiasNoiseFilter*eye(3) zeros(3);zeros(3) QuatNoiseFilter*eye(3)] */
// 	MatrixQ[0][0] = StrGyroFltNoiseQ1; MatrixQ[1][1] = StrGyroFltNoiseQ1; MatrixQ[2][2] = StrGyroFltNoiseQ1;
// 	MatrixQ[3][3] = StrGyroFltNoiseQ2; MatrixQ[4][4] = StrGyroFltNoiseQ2; MatrixQ[5][5] = StrGyroFltNoiseQ2;


// 	/* 预测P矩阵 GyroStarP_pre = Phi*GyroStarP_est*Phi'+Toi*Q*Toi' */
// 	mtxMtp((double*)tmp2, (double*)JacobianF, 6, 6, &tInterval, 1, 1);            /* tInterval*F2; */
// 	mtxCpy((double*)JacobianF_tmp, (double*)JacobianF, 6, 6);
// 	mtxMtp((double*)Fmatrix2, (double*)JacobianF, 6, 6, (double*)JacobianF_tmp, 6, 6);
// 	mtxMtp((double*)Fmatrix3, (double*)Fmatrix2, 6, 6, (double*)JacobianF, 6, 6);
// 	tInterval2 = 0.5 * tInterval * tInterval;
// 	mtxMtp((double*)tmp3, (double*)Fmatrix2, 6, 6, &tInterval2, 1, 1);
// 	tInterval3 = (1.0 / 6.0) * tInterval * tInterval * tInterval;
// 	mtxMtp((double*)tmp4, (double*)Fmatrix3, 6, 6, &tInterval3, 1, 1);
// 	mtxAdd((double*)tmp5, (double*)tmp2, (double*)tmp3, 6, 6);
// 	mtxAdd((double*)Phi, (double*)tmp5, (double*)tmp4, 6, 6);
// 	for (i = 0; i < 6; i++) {
// 		Phi[i][i] += 1.0;
// 	}
// 	mtxMtp((double*)tmp6, (double*)JacobianF, 6, 6, &tInterval2, 1, 1);
// 	mtxMtp((double*)tmp7, (double*)Fmatrix2, 6, 6, &tInterval3, 1, 1);
// 	tInterval4 = (1.0 / 24.0) * tInterval * tInterval * tInterval * tInterval;
// 	mtxMtp((double*)tmp8, (double*)Fmatrix3, 6, 6, &tInterval4, 1, 1);
// 	mtxAdd((double*)tmp9, (double*)tmp6, (double*)tmp7, 6, 6);
// 	mtxAdd((double*)tmp10, (double*)tmp9, (double*)tmp8, 6, 6);
// 	for (i = 0; i < 6; i++) {
// 		tmp10[i][i] += tInterval;
// 	}
// 	mtxMtp((double*)Toi, (double*)tmp10, 6, 6, (double*)MatrixG, 6, 6);
// 	mtxT((double*)Phi_t, (double*)Phi, 6, 6);
// 	mtxT((double*)Toi_t, (double*)Toi, 6, 6);
// 	mtxMtp((double*)tmp11, (double*)Phi, 6, 6, (double*)StrGyroFltP, 6, 6);
// 	mtxMtp((double*)Px, (double*)tmp11, 6, 6, (double*)Phi_t, 6, 6);
// 	mtxMtp((double*)tmp12, (double*)Toi, 6, 6, (double*)MatrixQ, 6, 6);
// 	mtxMtp((double*)tmp13, (double*)tmp12, 6, 6, (double*)Toi_t, 6, 6);
// 	mtxAdd((double*)StrGyroFltP, (double*)Px, (double*)tmp13, 6, 6);

// 	double norm_wbo_est = norm(Wbo_est, 3);
// 	double theta = cos(0.5 * norm_wbo_est * tInterval);

// 	double sin_term = sin(0.5 * norm_wbo_est * tInterval);
// 	double* pufai = (double*)malloc(sizeof(double) * 3);
// 	for (i = 0; i < 3; ++i) {
// 		pufai[i] = (sin_term * Wbo_est[i]) / norm_wbo_est;
// 	}
// 	CrossMatrixGet(pufaiX, pufai);

// 	for (i = 0; i < 3; i++) {
// 		for (j = 0; j < 3; j++) {
// 			Matrixtheta[i][j] = 0.0;
// 		}
// 	}
// 	Matrixtheta[0][0] = theta; Matrixtheta[1][1] = theta; Matrixtheta[2][2] = theta;
// 	mtxSub((double*)fai, (double*)Matrixtheta, (double*)pufaiX, 3, 3);

// 	omega[0][0] = theta; omega[0][1] = -pufai[0]; omega[0][2] = -pufai[1]; omega[0][3] = -pufai[2];
// 	omega[1][0] = pufai[0]; omega[1][1] = fai[0][0]; omega[1][2] = fai[0][1]; omega[1][3] = fai[0][2];
// 	omega[2][0] = pufai[1]; omega[2][1] = fai[1][0]; omega[2][2] = fai[1][1]; omega[2][3] = fai[1][2];
// 	omega[3][0] = pufai[2]; omega[3][1] = fai[2][0]; omega[3][2] = fai[2][1]; omega[3][3] = fai[2][2];

// 	double qq[4];
// 	for (i = 0; i < 4; i++)
// 	{
// 		qq[i] = StrGyroFltQ[i];
// 	}
// 	mtxMtp(StrGyroFltQ, (double*)omega, 4, 4, qq, 4, 1);

// 	ii = -1.0;
// 	if (StrGyroFltQ[0] < 0) {
// 		mtxMtp((double*)StrGyroFltQ, (double*)StrGyroFltQ, 4, 1, &ii, 1, 1);
// 	}

// 	double bb[3];
// 	for (i = 0; i < 3; i++)
// 	{
// 		bb[i] = StrGyroFltB[i];
// 	}
// 	for (i = 0; i < 3; i++)
// 	{
// 		StrGyroFltB[i] = bb[i];
// 	}
// }



/* 四元数平均值运算 */
void qAverage(double q[4], double a[4], double b[4]) {

	/* 盛一鸣，2023.06.20 */
	/* 四元数取平均值模块 */
	/* 输入量 */
	/* a  四元数1 */
	/* b  四元数2 */
	/* 输出量 */
	/* q  平均四元数 */

	double w1 = 0.5;
	double w2 = 0.5;
	double z_meas = sqrt(pow((w1 - w2), 2) + 4 * w1 * w2 * pow((a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]), 2));

	double factor1 = sqrt(w1 * (w1 - w2 + z_meas) / z_meas / (w1 + w2 + z_meas));
	double factor2 = sign(a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]) * sqrt(w2 * (w2 - w1 + z_meas) / z_meas / (w1 + w2 + z_meas));

	q[0] = factor1 * a[0] + factor2 * b[0];
	q[1] = factor1 * a[1] + factor2 * b[1];
	q[2] = factor1 * a[2] + factor2 * b[2];
	q[3] = factor1 * a[3] + factor2 * b[3];

	if (q[0] < 0)
	{
		q[0] = -q[0];
		q[1] = -q[1];
		q[2] = -q[2];
		q[3] = -q[3];
	}
}



/* 星敏感器测量数据坐标系转换 */
void STmeas2body(double qbi_meas[4], double qsi_meas[4], double StarQuatInstall[4]) {

	/* 盛一鸣，2023.06.20 */
	/* 星敏感器测量数据坐标系转换模块 */
	/* 输入量 */
	/* qsi_meas         星敏感器测量值 */
	/* StarQuatInstall  星敏感器实际位置到本体系的转换四元数 */
	/* 输出量 */
	/* qbi_meas         星敏感器相对于卫星本体系的测量值 */

	static double tmp1[4][4];

	qleft(tmp1, qsi_meas);
	mtxMtp((double*)qbi_meas, (double*)tmp1, 4, 4, (double*)StarQuatInstall, 4, 1);

	if (qbi_meas[0] < 0)
	{
		qbi_meas[0] = -qbi_meas[0];
		qbi_meas[1] = -qbi_meas[1];
		qbi_meas[2] = -qbi_meas[2];
		qbi_meas[3] = -qbi_meas[3];
	}
}



/* 陀螺仪测量数据坐标系转换 */
void GYmeas2body(double wbi_meas[3], double wgi_meas[3], double GyroMtxInstall[3][3]) {

	/* 盛一鸣，2023.06.20 */
	/* 陀螺仪测量数据坐标系转换模块 */
	/* 输入量 */
	/* wgi_meas        陀螺仪测量值 */
	/* GyroMtxInstall  陀螺仪实际位置到本体系的转换矩阵 */
	/* 输出量 */
	/* wbi_meas        陀螺仪相对于卫星本体系的测量值 */

	mtxMtp((double*)wbi_meas, (double*)GyroMtxInstall, 3, 3, (double*)wgi_meas, 3, 1);
}

/* 根据位置和速度信息推导惯性系到轨道系的转换矩阵 */
void J2O(double I2O[3][3], double PVInJ[6]) {

	/* 盛一鸣，2023.08.22 */
	/* 惯性系到轨道系转换矩阵计算模块 */
	/* 输入量 */
	/* PVInJ		惯性系下的位置和速度 */
	/* 输出量 */
	/* I2O			惯性系到轨道系的转换矩阵 */

	double PInJ[3] = { PVInJ[0], PVInJ[1], PVInJ[2] };
	double VInJ[3] = { PVInJ[3], PVInJ[4], PVInJ[5] };

	double normPInJ = sqrt(PInJ[0] * PInJ[0] + PInJ[1] * PInJ[1] + PInJ[2] * PInJ[2]);
	double Oz[3] = { -PInJ[0] / normPInJ, -PInJ[1] / normPInJ, -PInJ[2] / normPInJ };

	double normVInJ = sqrt(VInJ[0] * VInJ[0] + VInJ[1] * VInJ[1] + VInJ[2] * VInJ[2]);
	double V[3] = { VInJ[0] / normVInJ, VInJ[1] / normVInJ, VInJ[2] / normVInJ };

	double Oy_temp[3];
	Oy_temp[0] = Oz[1] * V[2] - Oz[2] * V[1];
	Oy_temp[1] = Oz[2] * V[0] - Oz[0] * V[2];
	Oy_temp[2] = Oz[0] * V[1] - Oz[1] * V[0];
	double normOy_temp = sqrt(Oy_temp[0] * Oy_temp[0] + Oy_temp[1] * Oy_temp[1] + Oy_temp[2] * Oy_temp[2]);
	double Oy[3] = { Oy_temp[0] / normOy_temp, Oy_temp[1] / normOy_temp, Oy_temp[2] / normOy_temp };

	double Ox[3];
	Ox[0] = Oy[1] * Oz[2] - Oy[2] * Oz[1];
	Ox[1] = Oy[2] * Oz[0] - Oy[0] * Oz[2];
	Ox[2] = Oy[0] * Oz[1] - Oy[1] * Oz[0];

	I2O[0][0] = Ox[0]; I2O[0][1] = Ox[1]; I2O[0][2] = Ox[2];
	I2O[1][0] = Oy[0]; I2O[1][1] = Oy[1]; I2O[1][2] = Oy[2];
	I2O[2][0] = Oz[0]; I2O[2][1] = Oz[1]; I2O[2][2] = Oz[2];
}



/* 由转换矩阵计算四元数 */
void A2q(double q[4], double A[3][3]) {

	/* 盛一鸣，2023.08.22 */
	/* 转换矩阵计算四元数模块 */
	/* 输入量 */
	/* A			转换矩阵 */
	/* 输出量 */
	/* q			四元数 */

	double C[3][3] = { {A[0][0], A[0][1], A[0][2]},
					  {A[1][0], A[1][1], A[1][2]},
					  {A[2][0], A[2][1], A[2][2]} };

	double c0 = 0.5 * sqrt(1 + C[0][0] + C[1][1] + C[2][2]);
	double c1 = 0.5 * sqrt(1 + C[0][0] - C[1][1] - C[2][2]);
	double c2 = 0.5 * sqrt(1 - C[0][0] + C[1][1] - C[2][2]);
	double c3 = 0.5 * sqrt(1 - C[0][0] - C[1][1] + C[2][2]);

	double q0, q1, q2, q3;

	if (c0 == 0) {
		q0 = c0;
		q1 = c1;
		q2 = c2;
		q3 = c3;
	}
	else {
		q0 = c0;
		q1 = (C[1][2] - C[2][1] >= 0) ? c1 : -c1;
		q2 = (C[2][0] - C[0][2] >= 0) ? c2 : -c2;
		q3 = (C[0][1] - C[1][0] >= 0) ? c3 : -c3;
	}

	q[0] = q0; q[1] = q1; q[2] = q2; q[3] = q3;
}



/* 四元数表示的姿态矩阵 */
void TransQ(double Q[3][3], double q[4]) {

	/* 盛一鸣，2023.08.22 */
	/* 姿态矩阵四元数表达模块 */
	/* 输入量 */
	/* q			四元数 */
	/* 输出量 */
	/* Q			姿态矩阵 */

	double q1[1] = { q[0] };
	double q2[1] = { q[1] };
	double q3[1] = { q[2] };
	double q4[1] = { q[3] };

	Q[0][0] = q1[0] * q1[0] + q2[0] * q2[0] - q3[0] * q3[0] - q4[0] * q4[0]; Q[0][1] = 2 * (q2[0] * q3[0] + q1[0] * q4[0]);								Q[0][2] = 2 * (q2[0] * q4[0] - q1[0] * q3[0]);
	Q[1][0] = 2 * (q2[0] * q3[0] - q1[0] * q4[0]);							 Q[1][1] = q1[0] * q1[0] - q2[0] * q2[0] + q3[0] * q3[0] - q4[0] * q4[0];   Q[1][2] = 2 * (q3[0] * q4[0] + q1[0] * q2[0]);
	Q[2][0] = 2 * (q2[0] * q4[0] + q1[0] * q3[0]);							 Q[2][1] = 2 * (q3[0] * q4[0] - q1[0] * q2[0]);								Q[2][2] = q1[0] * q1[0] - q2[0] * q2[0] - q3[0] * q3[0] + q4[0] * q4[0];

}



/* 四元数转欧拉角(312)角度制 */
void qua2euler(double euler_angle[3], double q[4], double euler_angle_last[3]) {

	/* 盛一鸣，2023.08.22 */
	/* 四元数转欧拉角模块 */
	/* 输入量 */
	/* q					四元数 */
	/* euler_angle_last		欧拉角 */
	/* 输出量 */
	/* euler_angle			欧拉角 */

	double Q[3][3];
	TransQ(Q, q);
	double angle_roll = asin(Q[1][2]) * 180.0 / 3.14159265358979323846;
	if (fabs(angle_roll) > 89.95) {
		angle_roll = sign(angle_roll) * 89.95;
		euler_angle[0] = euler_angle_last[0];
		euler_angle[1] = angle_roll;
		euler_angle[2] = euler_angle_last[2];
	}
	else {
		double angle_yaw = atan2(-Q[1][0], Q[1][1]) * 180.0 / 3.14159265358979323846;
		double angle_pitch = atan2(-Q[0][2], Q[2][2]) * 180.0 / 3.14159265358979323846;
		euler_angle[0] = angle_yaw;
		euler_angle[1] = angle_roll;
		euler_angle[2] = angle_pitch;
	}
}



/* qbo=qoi^(-1)×qbi */
void QuatObit2Body(double qbo[4], double qoi[4], double qbi[4]) {

	/* 盛一鸣，2023.08.22 */
	/* qbo计算模块 */
	/* 输入量 */
	/* qoi			惯性系相对于轨道系 */
	/* qbi			惯性系相对于本体系 */
	/* 输出量 */
	/* qbo			轨道系相对于本体系 */

	double qoi_n[4], Qleft[4][4];

	qinv(qoi_n, qoi);
	qleft(Qleft, qoi_n);
	mtxMtp((double*)qbo, (double*)Qleft, 4, 4, (double*)qbi, 4, 1);
	if (qbo[0] < 0) {
			qbo[0] = -qbo[0];
			qbo[1] = -qbo[1];
			qbo[2] = -qbo[2];
			qbo[3] = -qbo[3];
	}
}



/* wbo=wbi-Abo*woi */
void AngVelObit2Body(double wbo[3], double wbi[3], double OrbitVel, double qbo[4]) {

	/* 盛一鸣，2023.08.22 */
	/* wbo计算模块 */
	/* 输入量 */
	/* wbi			惯性系相对于本体系角速度 */
	/* OrbitVel  	轨道系角速度 */
	/* qbo  		轨道系相对于本体系四元数 */
	/* 输出量 */
	/* wbo			轨道系相对于本体系角速度 */

	double Abo[3][3], Wo[3], woi[3];

	TransQ(Abo, qbo);
	Wo[0] = 0; Wo[1] = -OrbitVel; Wo[2] = 0;
	mtxMtp((double*)woi, (double*)Abo, 3, 3, (double*)Wo, 3, 1);
	wbo[0] = wbi[0] - woi[0];
	wbo[1] = wbi[1] - woi[1];
	wbo[2] = wbi[2] - woi[2];
}



/* 挑选绝对值最大值 */
void findMaxAbsoluteValue(double* maxValue, double Vector[3], int size) {

	/* 盛一鸣，2023.08.22 */
	/* 挑选绝对值最大值 */
	/* 输入量 */
	/* Vector		代考向量 */
	/* size  		向量维度 */
	/* 输出量 */
	/* maxValue		最大值 */
	int i;
	*maxValue = fabs(Vector[0]);
	for (i = 1; i < size; i++) {
		double absoluteValue = fabs(Vector[i]);
		if (absoluteValue > *maxValue) {
			*maxValue = absoluteValue;
		}
	}
}



/* 赋值 */
void AttiResultValue(double AttiQuatEst[4], double AttiVelEst[3], double AttiEulerEst[3], double AttiEulerEstLast[3], double q[4], double w[3]) {

	/* 盛一鸣，2023.08.22 */
	/* 输入量 */
	/* AttiQuatEst		四元数结果 */
	/* AttiVelEst  		角速度结果 */
	/* AttiEulerEst  	欧拉角结果 */
	/* AttiEulerEstLast 欧拉角参考值 */
	/* 输出量 */
	/* q		        四元数 */
	/* w		        角速度 */
	/* EulerAngleEst 欧拉角 */

	double EulerAngleEst[3];

	AttiQuatEst[0] = q[0];
	AttiQuatEst[1] = q[1];
	AttiQuatEst[2] = q[2];
	AttiQuatEst[3] = q[3];
	AttiVelEst[0] = w[0];
	AttiVelEst[1] = w[1];
	AttiVelEst[2] = w[2];
	qua2euler(EulerAngleEst, AttiQuatEst, AttiEulerEstLast);
	AttiEulerEst[0] = EulerAngleEst[1] * PI / 180.0;
	AttiEulerEst[1] = EulerAngleEst[2] * PI / 180.0;
	AttiEulerEst[2] = EulerAngleEst[0] * PI / 180.0;
	AttiEulerEstLast[0] = EulerAngleEst[0];
	AttiEulerEstLast[1] = EulerAngleEst[1];
	AttiEulerEstLast[2] = EulerAngleEst[2];
}



/* 四元数单位化 */
void QuatNormalize(double Q_out[4], double Q_input[4]) {

	double temp1 = norm(Q_input,4);

	/* 四元数转单位四元数 */
	if (temp1 != 1.0){
		Q_out[0] = Q_input[0] / temp1;
		Q_out[1] = Q_input[1] / temp1;
		Q_out[2] = Q_input[2] / temp1;
		Q_out[3] = Q_input[3] / temp1;
	}
	else {
		Q_out[0] = Q_input[0];
		Q_out[1] = Q_input[1];
		Q_out[2] = Q_input[2];
		Q_out[3] = Q_input[3];
	}
}



/* 四元数标量取正 */
void QuatPositive(double Q_out[4], double Q_input[4]) {

	if (Q_input[0] < 0.0){
		Q_out[0] = -1.0 * Q_input[0];
		Q_out[1] = -1.0 * Q_input[1];
		Q_out[2] = -1.0 * Q_input[2];
		Q_out[3] = -1.0 * Q_input[3];
	}
	else {
		Q_out[0] = Q_input[0];
		Q_out[1] = Q_input[1];
		Q_out[2] = Q_input[2];
		Q_out[3] = Q_input[3];
	}
}



/* 求取误差四元数q1 * delta_q = q2*/
void GetDeltaQuat(double DeltaQuat[4], double Q1[4], double Q2[4]) {

	double Q1_n[4], Q1n_left[4][4];

	qinv(Q1_n, Q1);
	qleft(Q1n_left, Q1_n);
	mtxMtp((double*)DeltaQuat, (double*)Q1n_left, 4, 4, (double*)Q2, 4, 1);
	if (DeltaQuat[0] < 0) {
		DeltaQuat[0] = -DeltaQuat[0];
		DeltaQuat[1] = -DeltaQuat[1];
		DeltaQuat[2] = -DeltaQuat[2];
		DeltaQuat[3] = -DeltaQuat[3];
	}
}


