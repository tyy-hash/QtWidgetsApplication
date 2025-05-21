#include "QtWidgetsApplication.h"

QtWidgetsApplication::QtWidgetsApplication(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	//相机需要的显示控件
	m_scene = new QGraphicsScene(this);
	ui.imgView->setScene(m_scene);

	//保存路径
	ui.edtSaveDir->setText("D:/");

	//TCT检测显示需要
	tct_pixmapItem = new QGraphicsPixmapItem(tct_pixmap);
	tct_scene = new QGraphicsScene(this);
	tct_scene->addItem(tct_pixmapItem);

	//TCT模型处理的线程
	my_TCT = new TCT(this);
	m_infQthread = new QThread(nullptr);
	my_TCT->moveToThread(m_infQthread);

	//TCT模型处理需要的信号与槽
	connect(this, &QtWidgetsApplication::emit_img, my_TCT, &TCT::main_module);
	connect(my_TCT, &TCT::em_img, this, &QtWidgetsApplication::input_img);
	m_infQthread->start();
	 
}

QtWidgetsApplication::~QtWidgetsApplication()
{

}

void QtWidgetsApplication::on_pb_camera_clicked() {

	QString currentText = ui.CameraBox->currentText();       // 获取文本
	if (currentText == "haikang camera")
	{
		// 创建相机线程，和信号与槽
		cam_thread = new QThread(this);
		Hacm_worker = new haikang(this);
		Hacm_worker->moveToThread(cam_thread);
		connect(cam_thread, &QThread::started, Hacm_worker, &haikang::GrabThreadProcess);
		connect(Hacm_worker, &haikang::img_redy, this, &QtWidgetsApplication::do_hai_img_redy);
		connect(cam_thread, &QThread::finished, Hacm_worker, &haikang::finish);
		cam_thread->start();
		m_hwndDisplay = (HWND)ui.imgView->winId();
		setSlideSpinboxCheckboxSync();
	}
	if (currentText == "flir camera")
	{
		// 创建相机线程，和信号与槽
		cam_thread = new QThread(this);
		cam_worker = new M_camera(nullptr);
		cam_worker->moveToThread(cam_thread);
		connect(cam_thread, &QThread::started, cam_worker, &M_camera::do_start);//线程开始，让相机初始化
		connect(cam_worker, &M_camera::img_redy, this, &QtWidgetsApplication::do_img_redy);
		cam_thread->start();
		//让曝光，增益的Slide和Spinbox同步
		setSlideSpinboxCheckboxSync();
	}



}
//将相机传回来的图片进行显示
void QtWidgetsApplication::do_hai_img_redy(MV_FRAME_OUT stImageInfo, void *handle)
{
	HK_handle = handle;
	MV_DISPLAY_FRAME_INFO stDisplayInfo = {0};
	stDisplayInfo.hWnd = m_hwndDisplay;   //显示的句柄 这种显示方式相当于绑定句柄
	stDisplayInfo.pData = stImageInfo.pBufAddr;
	stDisplayInfo.nDataLen = stImageInfo.stFrameInfo.nFrameLen;
	stDisplayInfo.nWidth = stImageInfo.stFrameInfo.nWidth;
	stDisplayInfo.nHeight = stImageInfo.stFrameInfo.nHeight;
	stDisplayInfo.enPixelType = stImageInfo.stFrameInfo.enPixelType;
	MV_CC_DisplayOneFrame(handle, &stDisplayInfo);
	MV_CC_FreeImageBuffer(handle, &stImageInfo);
}

//将相机传回来的图片进行显示
void QtWidgetsApplication::do_img_redy(ImagePtr pResultImage)
{
	const size_t width = pResultImage->GetWidth();
	const size_t height = pResultImage->GetHeight();
	const unsigned char* pData = (const unsigned char*)pResultImage->GetData();
	// 根据像素格式创建 QImage
	QImage qImage(pData, static_cast<int>(width), static_cast<int>(height),
		static_cast<int>(width * pResultImage->GetBitsPerPixel() / 8),
		QImage::Format_RGB888);  // 根据实际情况调整格式
	
	m_scene->clear();  // 清除旧图像
	QPixmap pixmap = QPixmap::fromImage(qImage);
	pixmapItem = m_scene->addPixmap(pixmap);
	
	ui.imgView->fitInView(pixmapItem, Qt::KeepAspectRatio);
	ui.imgView->setScene(m_scene);
	ui.imgView->show();
}


int QtWidgetsApplication::resetExposure() {

	Spinnaker::GenApi::INodeMap& nodeMap = cam_worker->pCam->GetNodeMap();

	CEnumerationPtr exposureAuto = nodeMap.GetNode("ExposureAuto");
	exposureAuto->SetIntValue(exposureAuto->GetEntryByName("Continuous")->GetValue());
	 
	return 0;
} 


int QtWidgetsApplication::configureExposure(double exposure) {

	Spinnaker::GenApi::INodeMap& nodeMap = cam_worker->pCam->GetNodeMap();

	CEnumerationPtr exposureAuto = nodeMap.GetNode("ExposureAuto");
	exposureAuto->SetIntValue(exposureAuto->GetEntryByName("Off")->GetValue());

	CEnumerationPtr exposureMode = nodeMap.GetNode("ExposureMode");
	exposureMode->SetIntValue(exposureMode->GetEntryByName("Timed")->GetValue());

	CFloatPtr exposureTime = nodeMap.GetNode("ExposureTime");
	exposureTime->SetValue(exposure);

	return 0;
}


void QtWidgetsApplication::setSlideSpinboxCheckboxSync() {
	QString currentText = ui.CameraBox->currentText();
	if (currentText == "flir camera") {
		// Exposure
		connect(ui.sldExposure, &QSlider::valueChanged, ui.spinExposure, [this](int sliderValue) {
			ui.spinExposure->setValue(static_cast<double>(sliderValue));
			});
		connect(ui.spinExposure, QOverload<double>::of(&QDoubleSpinBox::valueChanged), ui.sldExposure, [this](double spinValue) {
			ui.sldExposure->setValue(static_cast<int>(spinValue));
			configureExposure(spinValue);
			});
		connect(ui.chkExposure, &QCheckBox::toggled, [this](bool checked) {
			bool isEnabled = !checked;
			ui.spinExposure->setEnabled(isEnabled);
			ui.sldExposure->setEnabled(isEnabled);
			resetExposure();
			});

		// Gain
		connect(ui.sldGain, &QSlider::valueChanged, ui.spinGain, [this](int sliderValue) {
			ui.spinGain->setValue(static_cast<double>(sliderValue) / 100.0);
			});
		connect(ui.spinGain, QOverload<double>::of(&QDoubleSpinBox::valueChanged), ui.sldGain, [this](double spinValue) {
			ui.sldGain->setValue(static_cast<int>(spinValue * 100));
			configureGain(spinValue);
			});
		connect(ui.chkGain, &QCheckBox::toggled, [this](bool checked) {
			bool isEnabled = !checked;
			ui.spinGain->setEnabled(isEnabled);
			ui.sldGain->setEnabled(isEnabled);
			resetGain();
			});
	}
	if (currentText == "haikang camera") {
		// Exposure
		connect(ui.sldExposure, &QSlider::valueChanged, ui.spinExposure, [this](int sliderValue) {
			ui.spinExposure->setValue(static_cast<double>(sliderValue));
			});
		connect(ui.spinExposure, QOverload<double>::of(&QDoubleSpinBox::valueChanged), ui.sldExposure, [this](double spinValue) {
			ui.sldExposure->setValue(static_cast<int>(spinValue));
			HK_configureExposure(spinValue);
			});
		connect(ui.chkExposure, &QCheckBox::toggled, [this](bool checked) {
			bool isEnabled = !checked;
			ui.spinExposure->setEnabled(isEnabled);
			ui.sldExposure->setEnabled(isEnabled);
			HK_resetExposure();
			});

		// Gain
		connect(ui.sldGain, &QSlider::valueChanged, ui.spinGain, [this](int sliderValue) {
			ui.spinGain->setValue(static_cast<double>(sliderValue) / 100.0);
			});
		connect(ui.spinGain, QOverload<double>::of(&QDoubleSpinBox::valueChanged), ui.sldGain, [this](double spinValue) {
			ui.sldGain->setValue(static_cast<int>(spinValue * 100));
			HK_configureGain(spinValue);
			});
		connect(ui.chkGain, &QCheckBox::toggled, [this](bool checked) {
			bool isEnabled = !checked;
			ui.spinGain->setEnabled(isEnabled);
			ui.sldGain->setEnabled(isEnabled);
			HK_resetGain();
			});
	}
	
}

void QtWidgetsApplication::HK_resetGain() {


	MV_CC_SetExposureAutoMode(HK_handle, 2);

}

void QtWidgetsApplication::HK_resetExposure() 
{
	MV_CC_SetGain(HK_handle, 2);
}

void QtWidgetsApplication::HK_configureExposure(double exposure)
{
	MV_CC_SetExposureAutoMode(HK_handle, 0);
	MV_CC_SetFloatValue(HK_handle, "ExposureTime", exposure);
}

void QtWidgetsApplication::HK_configureGain(double gain)
{
	MV_CC_SetGain(HK_handle, 0);
	MV_CC_SetFloatValue(HK_handle, "Gain", gain);
}

int QtWidgetsApplication::configureGain(double gain) {
	Spinnaker::GenApi::INodeMap& nodeMap = cam_worker->pCam->GetNodeMap();

	CEnumerationPtr gainAuto = nodeMap.GetNode("GainAuto");
	gainAuto->SetIntValue(gainAuto->GetEntryByName("Off")->GetValue());

	CFloatPtr gainValue = nodeMap.GetNode("Gain");
	gainValue->SetValue(gain);

	return 0;
}

int QtWidgetsApplication::resetGain() {
	Spinnaker::GenApi::INodeMap& nodeMap = cam_worker->pCam->GetNodeMap();

	CEnumerationPtr gainAuto = nodeMap.GetNode("GainAuto");
	gainAuto->SetIntValue(gainAuto->GetEntryByName("Continuous")->GetValue());

	return 0;
}

void QtWidgetsApplication::on_btnSingleImageSave_clicked() {

	qDebug() << "Single image save clicked";
	QString fileName = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz");
	QString absPath = ui.edtSaveDir->text() + "/" + fileName + ".tif";
	qDebug() << "Save path: " << absPath << "";

	ImagePtr pImage = cam_worker->pCam->GetNextImage(1000);
	if (pImage->IsIncomplete()) {
		qDebug() << "Image incomplete in 1s: " << Image::GetImageStatusDescription(pImage->GetImageStatus()) << "...";
	}
	ImagePtr pConvertedImage = processor.Convert(pImage, PixelFormat_RGB8);
	pConvertedImage->Save(absPath.toStdString().c_str());
}

void QtWidgetsApplication::on_btnSaveDir_clicked() {
	qDebug() << "Choose save dir clicked";
	QString dirPath = QFileDialog::getExistingDirectory(
		this, 
		QString::fromUtf8(u8"请选择图像保存路径")
	);
	qDebug() << "Choose save dir: " << dirPath << "";
	ui.edtSaveDir->setText(dirPath);
}


// 将TCT检测的图片进行显示
void QtWidgetsApplication::input_img(QImage image)
{
	in_image = image;
	// 显示经过网络检测后的图片
	this->ui.TCT_View->setScene(tct_scene);
	tct_pixmap = QPixmap::fromImage(in_image);
	tct_pixmapItem = tct_scene->addPixmap(tct_pixmap);
	// 自适应显示图片
	ui.TCT_View->fitInView(tct_pixmapItem, Qt::KeepAspectRatio);
	ui.TCT_View->setRenderHint(QPainter::Antialiasing);  // 抗锯齿
	ui.TCT_View->setDragMode(QGraphicsView::ScrollHandDrag);  // 启动手动拖动模式
	ui.TCT_View->show();
}

// 将ImagePtr转换成QImage
QImage convertToQImage(const ImagePtr& pImage)
{
	// 获取图像尺寸
	int width = pImage->GetWidth();  
	int height = pImage->GetHeight();

	// 获取像素数据指针（假设为 uchar* 类型）
	const uchar* pData = reinterpret_cast<const uchar*>(pImage->GetData());

	// 获取每行的字节数（步长）
	int stride = width * 3; // 假设存在此方法，若不存在可设为 width * 3

	// 构造 QImage
	QImage qImage(pData, width, height, stride, QImage::Format_RGB888);

	// 返回深拷贝，避免原始图像数据被释放导致 QImage 无效
	return qImage.copy();
}

// 将相机图片输入到网络的线程中 
void QtWidgetsApplication::on_pb_model_clicked()
{
	/*QString fileName = QFileDialog::getOpenFileName(this, "选择图片", "D:/TCT_down/dataset/test/images", "Images (*.png *.jpg)");
	QImage image(fileName);*/  
	ImagePtr pImage = cam_worker->pCam->GetNextImage(1000);
	if (pImage->IsIncomplete()) {
		qDebug() << "Image incomplete in 1s: " << Image::GetImageStatusDescription(pImage->GetImageStatus()) << "...";
	}
	ImagePtr pConvertedImage = processor.Convert(pImage, PixelFormat_RGB8);
	QImage image = convertToQImage(pConvertedImage);
	emit emit_img(image);
	
}