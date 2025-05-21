#include "M_camera.h"

M_camera::M_camera(QObject* parent)
{


}

M_camera::~M_camera()
{

}

//死循环，一直获取图像
void M_camera::do_start() {

	system = System::GetInstance();//新建相机系统
	camList = system->GetCameras();//获取相机列表
	const unsigned int numCameras = camList.GetSize();//获取相机数目
	cout << "Number of cameras detected: " << numCameras << endl << endl;
	pCam = camList.GetByIndex(0);//相机实例化，由于只有一个相机，因此相机index为0
	pCam->Init();//相机初始化
	pCam->BeginAcquisition();//开始采集图像
	while (1) {
		pResultImage = pCam->GetNextImage();
		pResultImage = processor.Convert(pResultImage, PixelFormat_RGB8);
		emit img_redy(pResultImage);
	}  	
}