#include "Mycontrol.h"


Mycontrol::Mycontrol()
{
}

// 获取单例实例（C++11 线程安全）
Mycontrol& Mycontrol::getInstance() {
    static Mycontrol instance;
    return instance;
}

void Mycontrol::init() {
    qDebug() << QString("Mycontrol: init() called");
}

void Mycontrol::moveBy(int motorID, double distance, int block) {
    qDebug() << QString("Mycontrol: moveBy(motorID=%1, distance=%2, block=%3)")
        .arg(motorID).arg(distance).arg(block);
    double nowPosition = 0.0;
    GetNowPos(motorID, nowPosition);
    SetPosModePos(motorID, nowPosition + distance);
    TillPosModePos(motorID, block);
}

void Mycontrol::moveTo(int motorID, double targetPosition, int block) {
    qDebug() << QString("Mycontrol: moveTo(motorID=%1, targetPosition=%2, block=%3)")
        .arg(motorID).arg(targetPosition).arg(block);
    SetPosModePos(motorID, targetPosition);
    TillPosModePos(motorID, block);
}

void Mycontrol::moveByX(double xDis) {
    moveBy(2, xDis);
}

void Mycontrol::moveByY(double yDis) {
    moveBy(1, yDis);
} 

void Mycontrol::moveByZ(double zDis) {
    moveBy(3, zDis);
}