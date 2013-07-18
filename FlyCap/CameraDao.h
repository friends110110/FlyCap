#pragma once
#include "FlyCapture2.h"
#include "FlyCapture2GUI.h" 
using namespace FlyCapture2;
class CCameraDao
{
public:
	CCameraDao(void);
	~CCameraDao(void);
	virtual bool InitCamera()=0;
	virtual bool isEmpty()=0;		//is exist
	virtual bool DelCamera()=0;
	
	virtual bool SelectCamera()=0;
	virtual bool StartCamera()=0;
	virtual bool StartCapture()=0;
	virtual bool connect(PGRGuid * arGuid)=0;
	virtual bool Disconnect()=0;
	virtual bool StopCapture()=0;
	virtual bool StopAndDisconnectCamera()=0;
	virtual bool RetrieveBuffer(Image *)=0;


	virtual bool GetCameraInfo(CameraInfo *)=0;
	virtual bool IsDataFlashSupported()=0;
	virtual bool PrintFormat7Capabilities(Format7Info *fmt7Info)=0;
	virtual bool CheckFormat7InfoAndSetCustom()=0;
	virtual bool PrintFrameCameraRate()=0;
	virtual bool PrintCameraInfo(CameraInfo *)=0;
	virtual bool PrintBuildInfo()=0;

	virtual bool IsCameracontrolToggleDialogVisible()=0;
	virtual bool CameracontrolToggleDialogHide()=0;
	virtual bool CameracontrolToggleDialogShow()=0;
	virtual bool CameracontrolToggleDialogConnect()=0;
	virtual bool CameracontrolToggleDialogDisconnect()=0;
};

