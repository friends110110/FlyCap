#pragma once
#include "CameraDao.h"
#include "FlyCapture2.h"
#include "FlyCapture2GUI.h" 
class CCameraDaoImpl:public CCameraDao
{
public:
	CCameraDaoImpl(void);
	~CCameraDaoImpl(void);

	 bool InitCamera();
	 bool isEmpty();		//is exist
	 bool DelCamera();

	 bool SelectCamera();
	 bool StartCamera();
	 bool StartCapture();
	 bool connect(PGRGuid * arGuid);
	 bool Disconnect();
	 bool StopCapture();
	 bool StopAndDisconnectCamera();
	 bool RetrieveBuffer(Image *);


	 bool GetCameraInfo(CameraInfo *);
	 bool IsDataFlashSupported();
	 bool PrintFormat7Capabilities(Format7Info *);
	 bool CheckFormat7InfoAndSetCustom();
	 bool PrintFrameCameraRate();
	 bool PrintCameraInfo(CameraInfo *);
	 bool PrintBuildInfo();




	 bool CameracontrolToggleDialogHide();
	 bool CameracontrolToggleDialogShow();
	 bool CameracontrolToggleDialogConnect();
	 bool CameracontrolToggleDialogDisconnect();
	 virtual bool IsCameracontrolToggleDialogVisible();
private:
	Camera *m_pCamera;
	CameraControlDlg *m_camCtlDlg;
	CameraInfo * m_cameraInfo;
	Error error;
	static const  Mode m_format7mode=MODE_1 ;	
	static const  PixelFormat m_format7pixel=PIXEL_FORMAT_RAW8 ;//PIXEL_FORMAT_RAW8 ;//PIXEL_FORMAT_RGB;//PIXEL_FORMAT_MONO8;//PIXEL_FORMAT_RAW8 ;//PIXEL_FORMAT_BGRU;//;

public:
	void PrintError(Error error);
};

