# 数据处理测试

1. 对数据处理模块的一些数学函数进行调用，看算力占用情况
2. 测试打包后的容器性能

✅ 测试目标
- 模拟传感器数据变化（陀螺仪、星敏感器）
- 调用数据处理函数进行姿态估计
- 模拟周期性执行（每 10ms 执行一次）

每 10ms 调用一次数据处理函数，模拟实时系统中的定时任务。使用 add_noise() 添加小幅度高斯噪声，模拟真实传感器测量值波动。敏感器四元数主值为 [1, 0, 0, 0]，其余三轴加入微小扰动。

所有结构体字段都进行了合理初始化，确保不会出现非法访问。标志位（如 .Flag, .OnFlag）保持活跃状态，便于观察正常流程。

## 微服务改造

将数据处理改造成服务端

将调用改造成客户端

选择通信模式为RPC

### RPC
#### 服务端开发
微服务对外提供RPC服务时，需要制定该RPC的URL资源标识并将该URL加入监听，同时需要制定RPC的回调函数用于处理RPC被调用时的逻辑。

```c
void GyroDataProcessing(FiberGyro Gyro1, MEMSGyro Gyro2, int OrbCtrlAllowFlag, double wbi_StarMeas[3]);
void StarDataProcessing(StarSensor Star1,StarSensor Star2, AttitudeData AttiData);
```

那么数据处理这个模块就是向外提供两个资源标识，供客户端RPC调用陀螺仪和星敏感器的数据处理服务

那么首先要将"数据处理"定义为一个`/dataproc`的资源标识，然后定义`/dataproc/gyro`和`/dataproc/star`两个子资源标识

##### 回调函数设计

客户端发送原始数据，服务器计算后返回结果；**客户端请求类型应为 RPC 调用或等价于 HTTP 的 POST 请求**

由于不清楚具体的业务逻辑，所以目前客户端和服务端之间通信的payload自定义如下,wbi_StarMeas由服务端默认填充：

```json
{
    "Gyro1": {
        "Flag": 1,
        "Meas": [0.01, 0.02, 0.03],
        "MtxInstall": [[1,0,0],[0,1,0],[0,0,1]],
        "FaultCnt": 0,
        "OnFlag": 1
    },
    "Gyro2": {
        "Flag": 1,
        "Meas": [0.02, 0.04, 0.06],
        "MtxInstall": [[1,0,0],[0,1,0],[0,0,1]],
        "FaultCnt": 0,
        "OnFlag": 1
    },
    "OrbCtrlAllowFlag": 1
}
```

 现在遇到一个问题，执行数据处理的这部分逻辑

```c
/* 角度制转弧度制输出 */
    wbi_meas[0] = wbi_meas[0] * PI/180.0; wbi_meas[1] = wbi_meas[1] * PI/180.0; wbi_meas[2] = wbi_meas[2] * PI/180.0;
    printf("wbi_meas: %f, %f, %f\n", wbi_meas[0], wbi_meas[1], wbi_meas[2]);
```

是直接在控制台打印结果，并没有返回值，也没有传递地址，所以对传递过来的参数并没有修改

**反序列化**

完成从json到结构体的反序列化，设计单元测试并通过

![image-20250528221030549](assets/image-20250528221030549.png)



##### 编译打包部署

编写完函数主体后，需要对工程进行编译，在编译前需要进行一些Include路径和链接路径的设置

- 首先把 [computility.c](javascript:void(0))、[selfmath.c](javascript:void(0))、[AttiMeasFun.c](javascript:void(0)) 这三个 C 源文件分别编译成 `.o` 文件，其实编译工程的时候已经编译过了，在`/DEBUG/obj/src`目录下

- 设置`dataproc_server`工程的Include Path

  ![image-20250528153200683](assets/image-20250528153200683.png)

- 设置`dataproc_server`工程的Linker Setting

  ![image-20250528153756543](assets/image-20250528153756543.png)

  ![image-20250528153806083](assets/image-20250528153806083.png)

现在是把dataproc的代码抽出来新建了一个动态链接库文件，需要上传到服务器上；静态链接好像直接在编译时直接连接到一起了，不需要上传。

目前动态链接后能够完成后续运行，暂时不修改

