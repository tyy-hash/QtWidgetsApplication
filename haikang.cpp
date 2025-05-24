#pragma once
#pragma execution_character_set("UTF-8")
#include <QtWidgets/QMainWindow>
#include "haikang.h"
#include <Image.h>



haikang::haikang(QWidget* parent)
{
    nRet = MV_CC_Initialize();
    // ch:枚举设备 | en:Enum device
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE | MV_GENTL_CAMERALINK_DEVICE | MV_GENTL_CXP_DEVICE | MV_GENTL_XOF_DEVICE, &stDeviceList);
    if (MV_OK != nRet)
    {
        printf("Enum Devices fail! nRet [0x%x]\n", nRet);

    }
    if (stDeviceList.nDeviceNum > 0)
    {
        for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
        {
            printf("[device %d]:\n", i);
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (NULL == pDeviceInfo)
            {
                break;
            }
            PrintDeviceInfo(pDeviceInfo);
        }
    }
    else
    {
        printf("Find No Devices!\n");

    }
    printf("Please Input camera index(0-%d):", stDeviceList.nDeviceNum - 1);
    unsigned int nIndex = 0;

    // ch:选择设备并创建句柄 | en:Select device and create handle
    nRet = MV_CC_CreateHandle(&handle, stDeviceList.pDeviceInfo[nIndex]);
    if (MV_OK != nRet)
    {
        printf("Create Handle fail! nRet [0x%x]\n", nRet);
    }

    // ch:打开设备 | en:Open device
    nRet = MV_CC_OpenDevice(handle);
    if (MV_OK != nRet)
    {
        printf("Open Device fail! nRet [0x%x]\n", nRet);
    }
    else
    {
        printf("Open Device success! nRet [0x%x]\n", nRet);
    }

    // ch:开始取流 | en:Start grab image
    nRet = MV_CC_StartGrabbing(handle);

}

haikang::~haikang()
{
    finish();

}

bool haikang::PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
    if (NULL == pstMVDevInfo)
    {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    
    else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
    {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
        printf("Device Number: %d\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.nDeviceNumber);
    }
    return true;
}



void haikang::GrabThreadProcess() {
    MV_CC_SetEnumValue(handle, "TriggerMode", MV_TRIGGER_MODE_OFF);
    while (1) {
        MV_CC_GetImageBuffer(handle, &stImageInfo, 1000);
        // 检查像素格式是否为Bayer RG8
        if (stImageInfo.stFrameInfo.enPixelType == PixelType_Gvsp_BayerRG8) {
            // 使用OpenCV处理Bayer数据
            cv::Mat bayerMat(
                stImageInfo.stFrameInfo.nHeight,
                stImageInfo.stFrameInfo.nWidth,
                CV_8UC1,
                stImageInfo.pBufAddr
            );
            cv::Mat rgbMat;
            cv::cvtColor(bayerMat, rgbMat, cv::COLOR_BayerRG2RGB);
            // 转换为QImage（RGB888）
            QImage tempImage(
                rgbMat.data,
                rgbMat.cols,
                rgbMat.rows,
                rgbMat.step,
                QImage::Format_RGB888
            );
            // 深拷贝数据确保安全
            QImage safeImage = tempImage.copy();
            // 发送信号到主线程显示
            emit img_to(safeImage, handle);
        }
        MV_CC_FreeImageBuffer(handle, &stImageInfo);
    }
}

//memcpy(&m_stImageInfo, &(stImageInfo.stFrameInfo), sizeof(MV_FRAME_OUT_INFO_EX));   //复制一份出来，用于保存图片
            /*QImage image(stImageInfo.pBufAddr,
                stImageInfo.stFrameInfo.nWidth,
                stImageInfo.stFrameInfo.nHeight,
                QImage::Format_Indexed8);

            int bytesPerLine = stImageInfo.stFrameInfo.enPixelType;
            qDebug() << "is: " << stImageInfo.stFrameInfo.enPixelType << "";
            printf("bytesPerLine");
            emit img_to(image, handle);
            MV_CC_FreeImageBuffer(handle, &stImageInfo);*/

            //MV_FRAME_OUT stOutFrame = stImageInfo;
            //emit img_redy(stImageInfo, handle); 


void haikang::finish() {

    nRet = MV_CC_StopGrabbing(handle);
    nRet = MV_CC_RegisterImageCallBackEx(handle, NULL, NULL);
    nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet)
    {
        printf("ClosDevice fail! nRet [0x%x]\n", nRet);
    }
    nRet = MV_CC_DestroyHandle(handle);
    if (MV_OK != nRet)
    {
        printf("Destroy Handle fail! nRet [0x%x]\n", nRet);
    }
    handle = NULL;
    if (handle != NULL)
    {
        MV_CC_DestroyHandle(handle);
        handle = NULL;
    }

    // ch:反初始化SDK | en:Finalize SDK
    MV_CC_Finalize();
    printf("Destroy Handle success! nRet [0x%x]\n", nRet);

}

