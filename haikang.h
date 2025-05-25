#pragma once
#include "MvCameraControl.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem> 
#include <cstdio>
#include <QPixmap> 
#include <QThread>
#include <QDebug>
#include <QImage>
#include <QLineEdit>
#include "opencv2/opencv.hpp"
#include <QDateTime>
#include <QMessageBox>
#include <QDir>
#include <QMutex>

class haikang : public QObject
{
    Q_OBJECT

public:
    haikang(QWidget* parent = nullptr);
    ~haikang();

    unsigned char* m_pSaveImageBuf;		// 保存图像缓冲区指针
    HWND  m_hwndDisplay;
    QMutex m_hSaveImageMux;
    int nRet = MV_OK;
    void* handle = NULL;
    bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo);
    
    MV_FRAME_OUT_INFO_EX	m_stImageInfo;  // 图像信息结构体变量
    MV_FRAME_OUT stImageInfo = {0};


public slots:
    void GrabThreadProcess();
    void finish();
signals:
    void img_redy(MV_FRAME_OUT stImageInfo, void* handle);

    void img_to(QImage img, void* handle);


};