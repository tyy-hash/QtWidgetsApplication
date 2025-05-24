#pragma once
#include <QThread> 
#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication.h"
#include <QGraphicsPixmapItem>
#include <QTimer.h>
#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>
#include <opencv2/opencv.hpp>
#include "M_camera.h"
#include "TCT.h"
#include <QSlider>
#include <QOverload>
#include <QCheckBox>
#include <QDateTime>
#include <QFileDialog>
#include "haikang.h"



class QtWidgetsApplication : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication(QWidget *parent = nullptr);

    ~QtWidgetsApplication();

    


    int resetExposure();                       // 自动曝光

    int configureExposure(double exposure);    //设置曝光 

    void setSlideSpinboxCheckboxSync();       //让曝光，增益的Slide和Spinbox同步

    void HK_resetGain();

    void HK_resetExposure();

    void HK_configureExposure(double exposure);

    void HK_configureGain(double exposure);

    int configureGain(double gain);            //设置增益 

    int resetGain();                          //自动增益

    QPixmap tct_pixmap;
    QGraphicsPixmapItem* tct_pixmapItem;
    QGraphicsScene* tct_scene;            //TCT检测显示需要的

    TCT* my_TCT;                          //创建模型处理的指针

    QThread* m_infQthread;                //模型处理的线程    

    QImage in_image;                      //传回来的图片

    QImage hikimage;
public slots:

    void do_img_redy(ImagePtr pResultImage);  //显示检测后的照片
    void on_btnSingleImageSave_clicked();     //保存单张图片
    void on_btnSaveDir_clicked();             //选择存储路径
    void input_img(QImage image);             //接收经过网络后的图片
    void on_pb_model_clicked();               //自动检测按钮
    void do_hai_img_redy(MV_FRAME_OUT stImageInfo, void* handle);
    void on_pb_camera_clicked();              //选择相机

    void hai_img_redy(QImage image, void* handle);
signals:
    void emit_img(QImage image);              //将需要传入网络的图片传出去


private:
    Ui::QtWidgetsApplicationClass ui;
    HWND  m_hwndDisplay;
    QPixmap pixmap;
    QGraphicsPixmapItem* pixmapItem;
    QGraphicsScene* m_scene;            //相机显示
    void* HK_handle = NULL;
    ImageProcessor processor;
    ImagePtr pResultImage;

    QThread* cam_thread;               // 相机的线程
    M_camera* cam_worker;              // filr相机
    haikang* Hacm_worker;              // haikang相机
};
