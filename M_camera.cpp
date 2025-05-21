#include "M_camera.h"

M_camera::M_camera(QObject* parent)
{


}

M_camera::~M_camera()
{

}

//��ѭ����һֱ��ȡͼ��
void M_camera::do_start() {

	system = System::GetInstance();//�½����ϵͳ
	camList = system->GetCameras();//��ȡ����б�
	const unsigned int numCameras = camList.GetSize();//��ȡ�����Ŀ
	cout << "Number of cameras detected: " << numCameras << endl << endl;
	pCam = camList.GetByIndex(0);//���ʵ����������ֻ��һ�������������indexΪ0
	pCam->Init();//�����ʼ��
	pCam->BeginAcquisition();//��ʼ�ɼ�ͼ��
	while (1) {
		pResultImage = pCam->GetNextImage();
		pResultImage = processor.Convert(pResultImage, PixelFormat_RGB8);
		emit img_redy(pResultImage);
	}  	
}