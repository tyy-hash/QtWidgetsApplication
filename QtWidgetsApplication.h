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

    


    int resetExposure();                       // �Զ��ع�

    int configureExposure(double exposure);    //�����ع� 

    void setSlideSpinboxCheckboxSync();       //���ع⣬�����Slide��Spinboxͬ��

    void HK_resetGain();

    void HK_resetExposure();

    void HK_configureExposure(double exposure);

    void HK_configureGain(double exposure);

    int configureGain(double gain);            //�������� 

    int resetGain();                          //�Զ�����

    QPixmap tct_pixmap;
    QGraphicsPixmapItem* tct_pixmapItem;
    QGraphicsScene* tct_scene;            //TCT�����ʾ��Ҫ��

    TCT* my_TCT;                          //����ģ�ʹ����ָ��

    QThread* m_infQthread;                //ģ�ʹ�����߳�    

    QImage in_image;                      //��������ͼƬ

    QImage hikimage;
public slots:

    void do_img_redy(ImagePtr pResultImage);  //��ʾ�������Ƭ
    void on_btnSingleImageSave_clicked();     //���浥��ͼƬ
    void on_btnSaveDir_clicked();             //ѡ��洢·��
    void input_img(QImage image);             //���վ���������ͼƬ
    void on_pb_model_clicked();               //�Զ���ⰴť
    void do_hai_img_redy(MV_FRAME_OUT stImageInfo, void* handle);
    void on_pb_camera_clicked();              //ѡ�����

    void hai_img_redy(QImage image, void* handle);
signals:
    void emit_img(QImage image);              //����Ҫ���������ͼƬ����ȥ


private:
    Ui::QtWidgetsApplicationClass ui;
    HWND  m_hwndDisplay;
    QPixmap pixmap;
    QGraphicsPixmapItem* pixmapItem;
    QGraphicsScene* m_scene;            //�����ʾ
    void* HK_handle = NULL;
    ImageProcessor processor;
    ImagePtr pResultImage;

    QThread* cam_thread;               // ������߳�
    M_camera* cam_worker;              // filr���
    haikang* Hacm_worker;              // haikang���
};
