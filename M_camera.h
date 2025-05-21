#pragma once
#include <QtWidgets/QMainWindow>
#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>
#include <opencv2/opencv.hpp>


using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;
using namespace cv;

class M_camera : public QObject {
    Q_OBJECT
public:

    M_camera(QObject* parent);
    ~M_camera();

    SystemPtr system;
    CameraList camList;
    CameraPtr pCam = nullptr;
    Mat frame;
    ImageProcessor processor;
    ImagePtr pResultImage;

public slots:
    void do_start();

signals:
    
    void img_redy(ImagePtr pResultImage);

private:
    
    

    
};

