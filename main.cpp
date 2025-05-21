#include "QtWidgetsApplication.h"
#include <QtWidgets/QApplication>
#include <opencv2/core/utils/logger.hpp>
 
int main(int argc, char *argv[])
{
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);//������־���
    QApplication a(argc, argv);
    QtWidgetsApplication w;
    w.show();
    return a.exec();
}
