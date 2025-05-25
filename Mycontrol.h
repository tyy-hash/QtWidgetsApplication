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
    Mycontrol();  // ˽�й��캯��
    Mycontrol(const Mycontrol&) = delete;
    Mycontrol& operator=(const Mycontrol&) = delete;

public:     
    static Mycontrol& getInstance();  // ��ȡ����ʵ��
    void init();                                                    // ��ʼ�� XT ����ϵͳ
    void moveBy(int motorID, double distance, int block = 0);       // �������ƶ���distance ������λΪ um
    void moveTo(int motorID, double targetPosition, int block = 0); // �������ƶ���targetPosition ������λΪ mm

    // �����ƶ��ӿ�
    void moveByX(double xDis);      // �������ƶ� X �ᣬ�����Ҹ�
    void moveByY(double yDis);      // �������ƶ� Y �ᣬǰ����
    void moveByZ(double zDis);      // �������ƶ� Z �ᣬ�����ϸ�

public slots:

};