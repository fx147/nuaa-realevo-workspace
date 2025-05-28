#include "computility.h"
#include "selfmath.h"
#include "AttiMeasFun.h"
/* 陀螺仪数据处理 */
void GyroDataProcessing(FiberGyro Gyro1, MEMSGyro Gyro2, int OrbCtrlAllowFlag, double wbi_StarMeas[3]) {

    /* 输入量 */
    /* MeasGyro1            光纤陀螺仪测量值 */
    /* MeasGyro2            微机械惯组测量值 */
    /* Gyro1MtxInstall      光纤陀螺仪安装标称矩阵 */
    /* Gyro2MtxInstall      微机械惯组安装标称矩阵 */

    /* 输出量 */
    /* wbi_meas             惯性系系相对于本体系的姿态角速度测量值 */
    
    // // 光纤陀螺
    // int Gyro1Flag;                               // 光纤陀螺仪有效标志位
    // double MeasGyro1[3];                         // 光纤陀螺仪测量值
    // double Gyro1MtxInstall[3][3];                // 光纤陀螺仪安装矩阵（实际到本体）
    // int Gyro1FaultCnt;
    // int Gyro1OnFlag;
    // // 微机械惯组
    // int Gyro2Flag;                               // 微机械惯组有效标志位
    // double MeasGyro2[3];                         // 微机械惯组测量值
    // double Gyro2MtxInstall[3][3];                // 微机械惯组安装矩阵（实际到本体）
    // int Gyro2FaultCnt;
    // int Gyro2OnFlag;
    // int OrbCtrlAllowFlag;                        // 轨控允许标志位
    // double wbi_StarMeas[3];						 // 星敏感器角速度测量值


    double wbi_meas[3];                          // 陀螺仪数据处理结果

    static double wgi_meas1[3] = {0,0,0};
    static double wgi_meas2[3] = {0,0,0};
    static int Gyro1ResetCnt = 0;
    static int Gyro2ResetCnt = 0;
    double normMeasGyro1, normMeasGyro2, DeltaGyro1Meas[3], DeltaGyro2Meas[3], NormDeltaGyro1Meas, NormDeltaGyro2Meas;

    /* 测量数据有效性判断 */
    // Gyro1Flag = 0;
    // normMeasGyro1 = norm(MeasGyro1, 3);
    // DeltaGyro1Meas[0] = MeasGyro1[0] - wgi_meas1[0];
    // DeltaGyro1Meas[1] = MeasGyro1[1] - wgi_meas1[1];
    // DeltaGyro1Meas[2] = MeasGyro1[2] - wgi_meas1[2];
    // NormDeltaGyro1Meas = norm(DeltaGyro1Meas, 3);
    // if ((normMeasGyro1 != 0) && (normMeasGyro1 < 100) && (NormDeltaGyro1Meas != 0) && (NormDeltaGyro1Meas < 20)){
    //     Gyro1Flag = 1;
    //     Gyro1FaultCnt = 0;
    // }
    // else if (Gyro1OnFlag == 1) {
    //     Gyro1FaultCnt++;
    // }
    Gyro1.Flag = 0;
    normMeasGyro1 = norm(Gyro1.Meas, 3);
    DeltaGyro1Meas[0] = Gyro1.Meas[0] - wgi_meas1[0];
    DeltaGyro1Meas[1] = Gyro1.Meas[1] - wgi_meas1[1];
    DeltaGyro1Meas[2] = Gyro1.Meas[2] - wgi_meas1[2];
    NormDeltaGyro1Meas = norm(DeltaGyro1Meas, 3);
    if ((normMeasGyro1 != 0) && (normMeasGyro1 < 100) && (NormDeltaGyro1Meas != 0) && (NormDeltaGyro1Meas < 20)) {
        Gyro1.Flag = 1;
        Gyro1.FaultCnt = 0;
    } else if (Gyro1.OnFlag == 1) {
        Gyro1.FaultCnt++;
    }


    // Gyro2Flag = 0;
    // normMeasGyro2 = norm(MeasGyro2, 3);
    // DeltaGyro2Meas[0] = MeasGyro2[0] - wgi_meas2[0];
    // DeltaGyro2Meas[1] = MeasGyro2[1] - wgi_meas2[1];
    // DeltaGyro2Meas[2] = MeasGyro2[2] - wgi_meas2[2];
    // NormDeltaGyro2Meas = norm(DeltaGyro2Meas, 3);
    // if ((normMeasGyro2 != 0) && (normMeasGyro2 < 100) && (NormDeltaGyro2Meas != 0) && (NormDeltaGyro1Meas < 20)){
    //     Gyro2Flag = 1;
    //     Gyro2FaultCnt = 0;
    // }
    // else if (Gyro2OnFlag == 1) {
    //     Gyro2FaultCnt++;
    // }
    Gyro2.Flag = 0;
    normMeasGyro2 = norm(Gyro2.Meas, 3);
    DeltaGyro2Meas[0] = Gyro2.Meas[0] - wgi_meas2[0];
    DeltaGyro2Meas[1] = Gyro2.Meas[1] - wgi_meas2[1];
    DeltaGyro2Meas[2] = Gyro2.Meas[2] - wgi_meas2[2];
    NormDeltaGyro2Meas = norm(DeltaGyro2Meas, 3);
    if ((normMeasGyro2 != 0) && (normMeasGyro2 < 100) && (NormDeltaGyro2Meas != 0) && (NormDeltaGyro2Meas < 20)) {
        Gyro2.Flag = 1;
        Gyro2.FaultCnt = 0;
    } else if (Gyro2.OnFlag == 1) {
        Gyro2.FaultCnt++;
    }



    /* 故障执行重启 */
    // if (Gyro1FaultCnt > 9){
    //     CloseGyro();                    //重启微光纤陀螺       10.24模飞临时屏蔽
    //     Gyro1ResetCnt++;
    //     Gyro1FaultCnt = 0;
    // }
    // else if (Gyro1ResetCnt == 1){
    //     OpenGyro();                     //10.24模飞临时屏蔽
    //     Gyro1ResetCnt = 0;
    //     Gyro1FaultCnt = 0;
    // }

    // if (Gyro2FaultCnt > 9){
    //     CloseMEMS();                    //重启微光纤陀螺       10.24模飞临时屏蔽
    //     Gyro2ResetCnt++;
    //     Gyro2FaultCnt = 0;
    //    }
    //    else if (Gyro2ResetCnt == 1){
    //     OpenMEMS();                     //10.24模飞临时屏蔽
    //     Gyro2ResetCnt = 0;
    //     Gyro2FaultCnt = 0;
    // }
    if (Gyro1.FaultCnt > 9) {
        //CloseGyro();
        printf("CloseGyro\n");
        Gyro1ResetCnt++;
        Gyro1.FaultCnt = 0;
    } else if (Gyro1ResetCnt == 1) {
        //OpenGyro();
        printf("OpenGyro\n");
        Gyro1ResetCnt = 0;
        Gyro1.FaultCnt = 0;
    }

    if (Gyro2.FaultCnt > 9) {
        //CloseMEMS();
        printf("CloseMEMS\n");
        Gyro2ResetCnt++;
        Gyro2.FaultCnt = 0;
    } else if (Gyro2ResetCnt == 1) {
        //OpenMEMS();
        printf("OpenMEMS\n");
        Gyro2ResetCnt = 0;
        Gyro2.FaultCnt = 0;
    }




    /* 选择测量数据输出 */
    // mtxCpy(wgi_meas1, MeasGyro1, 3, 1);
    // mtxCpy(wgi_meas2, MeasGyro2, 3, 1);
    // if (Gyro1Flag == 1) {
    //     GYmeas2body(wbi_meas, wgi_meas1, Gyro1MtxInstall);
    // }
    // else if (Gyro2Flag == 1) {
    //     GYmeas2body(wbi_meas, wgi_meas2, Gyro2MtxInstall);
    // }
    // else {
    //     mtxCpy(wbi_meas, wbi_StarMeas, 3, 1);
    //     OrbCtrlAllowFlag = 0;            //轨控停控标志位
    // }
    mtxCpy(wgi_meas1, Gyro1.Meas, 3, 1);
    mtxCpy(wgi_meas2, Gyro2.Meas, 3, 1);
    if (Gyro1.Flag == 1) {
        GYmeas2body(wbi_meas, wgi_meas1, Gyro1.MtxInstall);
    } else if (Gyro2.Flag == 1) {
        GYmeas2body(wbi_meas, wgi_meas2, Gyro2.MtxInstall);
    } else {
        mtxCpy(wbi_meas, wbi_StarMeas, 3, 1);
        OrbCtrlAllowFlag = 0;
    }

    /* 角度制转弧度制输出 */
    wbi_meas[0] = wbi_meas[0] * PI/180.0; wbi_meas[1] = wbi_meas[1] * PI/180.0; wbi_meas[2] = wbi_meas[2] * PI/180.0;
    printf("wbi_meas: %f, %f, %f\n", wbi_meas[0], wbi_meas[1], wbi_meas[2]);
    /* 输出结果 */
}

/* 星敏感器数据处理 */
void StarDataProcessing(StarSensor Star1,StarSensor Star2, AttitudeData AttiData) {

	/* 输入量 */
	/* MeasStar1			星敏感器1测量值 */
	/* MeasStar2			星敏感器2测量值 */
	/* Star1QuatInstall     星敏感器1安装标称矩阵 */
	/* Star2QuatInstall     星敏感器2安装标称矩阵 */
	/* 输出量 */
	/* qbi_meas			         惯性系相对于本体系的姿态四元数测量值 */
	/* StarValidFlag		星敏读数有效标志位 */
    // // 星敏1
    // int Star1Flag;                               // 星敏感器1有效标志位
    // double MeasStar1[4];                         // 星敏感器1测量值
    // double MeasStar1Vel[3];						 // 星敏感器1角速度测量值
    // double Star1QuatInstall[4];                  // 星敏感器1安装矩阵（实际到本体）
    // double Star1MtxInstall[3][3];
    // int Star1StarNum;
    // int Star1FaultCnt;
    // int Star1OnFlag;
    // // 星敏2
    // int Star2Flag;                               // 星敏感器2有效标志位
    // double MeasStar2[4];                         // 星敏感器2测量值
    // double MeasStar2Vel[3];						 // 星敏感器2角速度测量值
    // double Star2QuatInstall[4];                  // 星敏感器2安装矩阵（实际到本体）
    // double Star2MtxInstall[3][3];
    // int Star2StarNum;
    // int Star2FaultCnt;
    // int Star2OnFlag;

    // double QuatOrbit[4];                          // 惯性系相对于轨道系四元数
    // int AttiFiltValidFlag;                        // 滤波器收敛标志位
    // double AttiQuatEst[4];                        // 滤波器估计四元数
    // double QuatAverage[4];                        // 四元数平均值
    // double wbi_StarMeas[3];						  // 星敏感器角速度测量值
    // int OrbCtrlAllowFlag;                         // 轨控允许标志位

    double qbi_meas[4];                           // 星敏感器读数处理结果
    int StarValidFlag;                            // 星敏感器测量数据有效标志位

	double qsi_meas1[4], qsi_meas2[4], qbi_meas1[4], qbi_meas2[4], MtxQuatOrbit[4][4], QuatECI2Body[4], wbi_Star1Meas[3], wbi_Star2Meas[3];
	double temp1qsi[4], temp2qsi[4], temp_qbi[4], deltaQ1[4], deltaQ2[4], Error1, Error2, normDeltaStar1Meas, normDeltaStar2Meas;
	//JC
	normDeltaStar1Meas = 0;
	normDeltaStar2Meas = 0;
	static int StarResetCnt = 0;
	static int Star1MeasSameCnt = 0;
	static int Star2MeasSameCnt = 0;
	static double qsi1[4] = {1,0,0,0};
	static double qsi2[4] = {1,0,0,0};
	QuatECI2Body[0] = 1;QuatECI2Body[1] = 0;QuatECI2Body[2] = 0;QuatECI2Body[3] = 0;

	/* 测量数据坐标系转换 */
    if (Star1.Flag == 1) {
        deltaQ1[0] = Star1.Meas[0] - qsi1[0];
        deltaQ1[1] = Star1.Meas[1] - qsi1[1];
        deltaQ1[2] = Star1.Meas[2] - qsi1[2];
        deltaQ1[3] = Star1.Meas[3] - qsi1[3];
        normDeltaStar1Meas = norm(deltaQ1, 4);
        mtxCpy(qsi1, Star1.Meas, 4, 1);
        QuatNormalize(temp1qsi, Star1.Meas);
        QuatPositive(qsi_meas1, temp1qsi);
        STmeas2body(qbi_meas1, qsi_meas1, Star1.QuatInstall);
        GYmeas2body(wbi_Star1Meas, Star1.MeasVel, Star1.MtxInstall);
    }
	// if (Star1Flag == 1){
	// 	deltaQ1[0] = MeasStar1[0] - qsi1[0];
	// 	deltaQ1[1] = MeasStar1[1] - qsi1[1];
	// 	deltaQ1[2] = MeasStar1[2] - qsi1[2];
	// 	deltaQ1[3] = MeasStar1[3] - qsi1[3];
	// 	normDeltaStar1Meas = norm(deltaQ1, 4);
	// 	mtxCpy(qsi1, MeasStar1, 4, 1);
	// 	QuatNormalize(temp1qsi, MeasStar1);
	// 	QuatPositive(qsi_meas1, temp1qsi);
	// 	STmeas2body(qbi_meas1, qsi_meas1, Star1QuatInstall);
	// 	GYmeas2body(wbi_Star1Meas, MeasStar1Vel, Star1MtxInstall);
	// }
    if (Star2.Flag == 1) {
        deltaQ2[0] = Star2.Meas[0] - qsi2[0];
        deltaQ2[1] = Star2.Meas[1] - qsi2[1];
        deltaQ2[2] = Star2.Meas[2] - qsi2[2];
        deltaQ2[3] = Star2.Meas[3] - qsi2[3];
        normDeltaStar2Meas = norm(deltaQ2, 4);
        mtxCpy(qsi2, Star2.Meas, 4, 1);
        QuatNormalize(temp2qsi, Star2.Meas);
        QuatPositive(qsi_meas2, temp2qsi);
        STmeas2body(qbi_meas2, qsi_meas2, Star2.QuatInstall);
        GYmeas2body(wbi_Star2Meas, Star2.MeasVel, Star2.MtxInstall);
    }

	// if (Star2Flag == 1){
	// 	deltaQ2[0] = MeasStar2[0] - qsi2[0];
	// 	deltaQ2[1] = MeasStar2[1] - qsi2[1];
	// 	deltaQ2[2] = MeasStar2[2] - qsi2[2];
	// 	deltaQ2[3] = MeasStar2[3] - qsi2[3];
	// 	normDeltaStar2Meas = norm(deltaQ2, 4);
	// 	mtxCpy(qsi2, MeasStar2, 4, 1);
	// 	QuatNormalize(temp2qsi, MeasStar2);
	// 	QuatPositive(qsi_meas2, temp2qsi);
	// 	STmeas2body(qbi_meas2, qsi_meas2, Star2QuatInstall);
	// 	GYmeas2body(wbi_Star2Meas, MeasStar2Vel, Star2MtxInstall);
	// }

	/* 异常数据剔除 */
    qleft(MtxQuatOrbit, AttiData.QuatOrbit);
    mtxMtp((double*)temp_qbi, (double*)MtxQuatOrbit, 4, 4, (double*)AttiData.AttiQuatEst, 4, 1);
    QuatPositive(QuatECI2Body, temp_qbi);
    if (AttiData.AttiFiltValidFlag == 1) {
        if (Star1.Flag == 1) {
            GetDeltaQuat(deltaQ1, qbi_meas1, QuatECI2Body); // 求误差四元数deltaQ1
            Error1 = sqrt(deltaQ1[1] * deltaQ1[1] + deltaQ1[2] * deltaQ1[2] + deltaQ1[3] * deltaQ1[3]);
            if (Error1 > 0.03) {
                Star1.Flag = 2;
            }
        }
        if (Star2.Flag == 1) {
            GetDeltaQuat(deltaQ2, qbi_meas2, QuatECI2Body); // 求误差四元数deltaQ2
            Error2 = sqrt(deltaQ2[1] * deltaQ2[1] + deltaQ2[2] * deltaQ2[2] + deltaQ2[3] * deltaQ2[3]);
            if (Error2 > 0.03) {
                Star2.Flag = 2;
            }
        }
    }

	// qleft(MtxQuatOrbit, QuatOrbit);
	// mtxMtp((double*)temp_qbi, (double*)MtxQuatOrbit, 4, 4, (double*)AttiQuatEst, 4, 1);
	// QuatPositive(QuatECI2Body, temp_qbi);
	// if (AttiFiltValidFlag == 1){
	// 	if (Star1Flag == 1) {
	// 		GetDeltaQuat(deltaQ1, qbi_meas1, QuatECI2Body);//求误差四元数deltaQ1
	// 		Error1 = sqrt(deltaQ1[1]*deltaQ1[1] + deltaQ1[2]*deltaQ1[2] + deltaQ1[3]*deltaQ1[3]);
	// 		if (Error1 > 0.03){
	// 			Star1Flag = 2;
	// 		}
	// 	}
	// 	if (Star2Flag == 1) {
	// 		GetDeltaQuat(deltaQ2, qbi_meas2, QuatECI2Body);//求误差四元数deltaQ2
	// 		Error2 = sqrt(deltaQ2[1]*deltaQ2[1] + deltaQ2[2]*deltaQ2[2] + deltaQ2[3]*deltaQ2[3]);
	// 		if (Error2 > 0.03){
	// 			Star2Flag = 2;
	// 		}
	// 	}
	// }

	// 星敏1测量值刷新判断
    if ((Star1.Flag == 1) && (normDeltaStar1Meas == 0)) {
        Star1MeasSameCnt++;
    } else {
        Star1MeasSameCnt = 0;
    }
    if (Star1MeasSameCnt > 3) {
        Star1.Flag = 2;
    }

	// if ((Star1Flag == 1) && (normDeltaStar1Meas == 0)) {
	// 	Star1MeasSameCnt++;
	// }
	// else {
	// 	Star1MeasSameCnt = 0;
	// }
	// if (Star1MeasSameCnt > 3){
	// 	Star1Flag = 2;
	// }

	// 星敏2测量值刷新判断
    if ((Star2.Flag == 1) && (normDeltaStar2Meas == 0)) {
        Star2MeasSameCnt++;
    } else {
        Star2MeasSameCnt = 0;
    }
    if (Star2MeasSameCnt > 3) {
        Star2.Flag = 2;
    }
	// if ((Star2Flag == 1) && (normDeltaStar2Meas == 0)) {
	// 	Star2MeasSameCnt++;
	// }
	// else {
	// 	Star2MeasSameCnt = 0;
	// }
	// if (Star2MeasSameCnt > 3){
	// 	Star2Flag = 2;
	// }

	/* 选择测量数据输出 */
    if (Star1.Flag == 1 && Star2.Flag == 1) {
        qAverage(qbi_meas, qbi_meas1, qbi_meas2);
        AttiData.QuatAverage[0] = qbi_meas[0];
        AttiData.QuatAverage[1] = qbi_meas[1];
        AttiData.QuatAverage[2] = qbi_meas[2];
        AttiData.QuatAverage[3] = qbi_meas[3];
        mtxCpy(AttiData.wbi_StarMeas, wbi_Star1Meas, 3, 1);
        StarValidFlag = 1;
        Star1.FaultCnt = 0;
        Star2.FaultCnt = 0;
    } else if (Star1.Flag == 1) {
        qbi_meas[0] = qbi_meas1[0];
        qbi_meas[1] = qbi_meas1[1];
        qbi_meas[2] = qbi_meas1[2];
        qbi_meas[3] = qbi_meas1[3];
        mtxCpy(AttiData.wbi_StarMeas, wbi_Star1Meas, 3, 1);
        StarValidFlag = 1;
        Star1.FaultCnt = 0;
        Star2.FaultCnt++;
    } else if (Star2.Flag == 1) {
        qbi_meas[0] = qbi_meas2[0];
        qbi_meas[1] = qbi_meas2[1];
        qbi_meas[2] = qbi_meas2[2];
        qbi_meas[3] = qbi_meas2[3];
        mtxCpy(AttiData.wbi_StarMeas, wbi_Star2Meas, 3, 1);
        StarValidFlag = 1;
        Star2.FaultCnt = 0;
        Star1.FaultCnt++;
    } else {
        qbi_meas[0] = QuatECI2Body[0];
        qbi_meas[1] = QuatECI2Body[1];
        qbi_meas[2] = QuatECI2Body[2];
        qbi_meas[3] = QuatECI2Body[3];
        AttiData.wbi_StarMeas[0] = 0;
        AttiData.wbi_StarMeas[1] = 0;
        AttiData.wbi_StarMeas[2] = 0;
        StarValidFlag = 0;
        Star1.FaultCnt++;
        Star2.FaultCnt++;
    }
	// if (Star1Flag == 1 && Star2Flag == 1) {
	// 	qAverage(qbi_meas, qbi_meas1, qbi_meas2);
	// 	QuatAverage[0] = qbi_meas[0]; QuatAverage[1] = qbi_meas[1]; QuatAverage[2] = qbi_meas[2]; QuatAverage[3] = qbi_meas[3];
	// 	mtxCpy(wbi_StarMeas, wbi_Star1Meas, 3, 1);
	// 	StarValidFlag = 1;
	// 	Star1FaultCnt = 0;
	// 	Star2FaultCnt = 0;
	// }
	// else if (Star1Flag == 1) {
	// 	qbi_meas[0] = qbi_meas1[0]; qbi_meas[1] = qbi_meas1[1]; qbi_meas[2] = qbi_meas1[2]; qbi_meas[3] = qbi_meas1[3];
	// 	mtxCpy(wbi_StarMeas, wbi_Star1Meas, 3, 1);
	// 	StarValidFlag = 1;
	// 	Star1FaultCnt = 0;
	// 	Star2FaultCnt ++;
	// }
	// else if (Star2Flag == 1) {
	// 	qbi_meas[0] = qbi_meas2[0]; qbi_meas[1] = qbi_meas2[1]; qbi_meas[2] = qbi_meas2[2]; qbi_meas[3] = qbi_meas2[3];
	// 	mtxCpy(wbi_StarMeas, wbi_Star2Meas, 3, 1);
	// 	StarValidFlag = 1;
	// 	Star2FaultCnt = 0;
	// 	Star1FaultCnt ++;
	// }
	// else {
	// 	qbi_meas[0] = QuatECI2Body[0]; qbi_meas[1] = QuatECI2Body[1]; qbi_meas[2] = QuatECI2Body[2]; qbi_meas[3] = QuatECI2Body[3];
	// 	wbi_StarMeas[0] = 0; wbi_StarMeas[1] = 0; wbi_StarMeas[2] = 0;
	// 	StarValidFlag = 0;
	// 	Star1FaultCnt ++;
	// 	Star2FaultCnt ++;
	// }
    
    printf("qbi_meas: %f, %f, %f, %f\n", qbi_meas[0], qbi_meas[1], qbi_meas[2], qbi_meas[3]);
    /* 角度制转弧度制输出;惯性系相对于本体系的姿态四元数测量值 */
    printf("StarValidFlag: %d\n", StarValidFlag);
    /* 星敏读数有效标志位 */


	// /* 故障执行重启 */
	// if (Star1FaultCnt > 9 && Star2FaultCnt > 9 && get_CarryFlag() == CARRIED_NONE){
	// 	CloseStarsensor();                   //10.24模飞临时屏蔽
	// 	OrbCtrlAllowFlag = 0;
	// 	Star1FaultCnt = 0;
	// 	Star2FaultCnt = 0;
	// 	StarResetCnt++;
	// }
	// else if (StarResetCnt == 1){
	// 	OpenStarsensor();                   //10.24模飞临时屏蔽
	// 	OrbCtrlAllowFlag = 0;
	// 	Star1FaultCnt = 0;
	// 	Star2FaultCnt = 0;
	// 	StarResetCnt = 0;
	// }
}