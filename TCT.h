#pragma once
#pragma once
#include <QtWidgets/QMainWindow>
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QPixmap>
#include <QImage>
// Cpp native
#include <fstream>
#include <vector>
#include <string>
#include <random>

// OpenCV / DNN / Inference
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>



struct Detection
{
    int class_id{ 0 };
    std::string className{};
    float confidence{ 0.0 };
    cv::Scalar color{};
    cv::Rect box{};
};

class TCT : public QObject
{
    Q_OBJECT

public:
    TCT(QObject* parent = nullptr);
    ~TCT();

    // 处理onnx模型（传入onnx模型地址，网络所需图片size, classes, 是否使用GPU）
    TCT(const std::string& onnxModelPath, const cv::Size& modelInputShape = { 640, 640 }, const std::string& classesTxtFile = "", const bool& runWithCuda = false);
    std::vector<Detection> runInference(const cv::Mat& input);

    QImage img_i;

public slots:

    //总体处理函数
    void main_module(QImage image);

signals:

    //将经过网络的图片发给主线程
    void em_img(QImage img);

private:

    void loadClassesFromFile();

    //读取模型
    void loadOnnxNetwork();

    cv::Mat formatToSquare(const cv::Mat& source);

    std::string modelPath{};

    std::string classesPath{};

    bool cudaEnabled{};


    /*"bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush" };*/
    std::vector<std::string> classes{ "cancer" };   //类别数及其名称
    cv::Size2f modelShape{};

    float modelConfidenceThreshold{ 0.25 };  //置信度

    float modelScoreThreshold{ 0.15 };   //得分门槛

    float modelNMSThreshold{ 0.50 };    //最大值抑制

    bool letterBoxForSquare = true;

    cv::dnn::Net net;                   //定义的网络

};