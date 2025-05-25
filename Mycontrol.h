#pragma once
#include <QObject>
#include <QThread>
#include <QVector>
#include <QDebug>
#include <QQueue>
#include <QMutex>

#include <C_SoftLanding_dll.h>
#include <VCM_init_Struct.h>
#include <XT_MotionControlerExtend_Client_Lib.h>



class Mycontrol : public  QObject {
    Q_OBJECT



private:
    Mycontrol();  // 私有构造函数
    Mycontrol(const Mycontrol&) = delete;
    Mycontrol& operator=(const Mycontrol&) = delete;

public:     
    static Mycontrol& getInstance();  // 获取单例实例
    void init();                                                    // 初始化 XT 控制系统
    void moveBy(int motorID, double distance, int block = 0);       // 按距离移动，distance 参数单位为 um
    void moveTo(int motorID, double targetPosition, int block = 0); // 按坐标移动，targetPosition 参数单位为 mm

    // 分轴移动接口
    void moveByX(double xDis);      // 按距离移动 X 轴，左正右负
    void moveByY(double yDis);      // 按距离移动 Y 轴，前正后负
    void moveByZ(double zDis);      // 按距离移动 Z 轴，下正上负

public slots:

};