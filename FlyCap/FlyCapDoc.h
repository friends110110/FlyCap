
// FlyCapDoc.h : interface of the CFlyCapDoc class
//


#pragma once

#include "frameratecounter.h"

using namespace FlyCapture2;
//--------used for 

#include <vector>
using namespace std;
#include "picalgorithm.h"
#include "ImageDao.h"
#include "ImageDaoImpl.h"
#include "ImageProcess.h"
#include "CameraDaoImpl.h"
#include "FindKeyPositonDao.h"
#include "FindKeyPositionDaoImpl.h"
#include "KeyLocation.h"


//#include "MainFrm.h"
//#include "FlyCapView.h"




class CFlyCapDoc : public CDocument
{
protected: // create from serialization only
	CFlyCapDoc();
	DECLARE_DYNCREATE(CFlyCapDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CFlyCapDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:



	CCriticalSection m_csData;

	// .... my define....

private:
//	void PrintBuildInfo(void);
//	void PrintCameraInfo(CameraInfo *pCamInfo);
	static void PrintError(Error error);
//	bool IsDataFlashSupported(void);					//maybe unused
//	void PrintFormat7Capabilities(Format7Info fmt7Info); // customer size setting
//	void PrintFrameCameraRate();

public:
//	bool InitCamera(void);

	bool CaptureBackgroundImagePreProcess(void);
	unsigned char* GetProcessedPixels(void);

	bool saveCaptureImageToDisk(void);
//	bool StopAndDisconnectCamera(void);

//	bool CheckFormat7InfoAndSetCustom();			//CHECK and set custome attribute or the default setting
//	bool StartCamera(void);

	bool SetFrameRate(double framerate);
	void SaveAviHelper(	std::vector<Image>& vecImages, std::string aviFileName, float frameRate);

public:												//used for thread
	bool StartCapturePicThread(void);
	static UINT CapturePicThreadProc(LPVOID pparam);
	bool StopCapturePicThread(void);

	bool IsGrabThreadRunning(void);

	UINT DoGrabLoop(void);


	void RedrawAllViews(void);
	void InitBitmapStruct(int cols, int rows);
	double GetProcessedFrameRate(void);
	void GetImageSize(unsigned int * pWidth, unsigned int * pHeight);


public:

	BITMAPINFO m_bitmapInfo;
protected:
//	CameraControlDlg m_camCtlDlg;



private:
	// .... used for camera................
	CCameraDao *m_camera;
//	Camera *m_pCamera;
	Error error;
	//some essential operation return 
//	CameraInfo m_cameraInfo;

	//

	//....... used for thread.............
	CWinThread *m_grabLoopThread;
	bool m_grabLoopThreadFlag;
	HANDLE m_heventThreadDone;


	//.........used for image...........
	Image m_rawImage;
	Image m_backgroundImage;					//used for minus ,in order to get the information we want
	Image m_processedImage;
	Image m_grayImage;
	Image m_afterBackgroundMinusImage;
	Image m_2BImage;
	Image m_showImage;
public:
	CImageDao *m_imageDao;

public:
	Image *m_projectImage;
	Image *m_projectBackupImage;
	FrameRateCounter m_processedFrameRate;
	int m_projectImageSeparateLocationy;

	//��m_keypositionVector����ȡ����������ʾ��
	CString m_keyClickedboard;

private:
	//used for find key location
	CFindKeyPositonDao *m_keylocationDao;
public:
	vector<CKeyLocation> *m_keypositionVector;



	//...............used for const
// 	static const  Mode m_format7mode=MODE_1 ;	
// 	static const  PixelFormat m_format7pixel=PIXEL_FORMAT_RAW8 ;//PIXEL_FORMAT_RAW8 ;//PIXEL_FORMAT_RGB;//PIXEL_FORMAT_MONO8;//PIXEL_FORMAT_RAW8 ;//PIXEL_FORMAT_BGRU;//;
	static const int _DEFAULT_WINDOW_X  =640;
	static const int _DEFAULT_WINDOW_Y  =480;
public:
	char *shownumbers;
	CPicAlgorithm m_picalgorithm;
	unsigned char * GetGRAYPixels(void);

	unsigned char * Get2BImagePixels(void);
	unsigned char * getShowPixels(void);
	bool m_sensorflag;

	//ȷ�����ĵ���ͼƬ�Ȼ�ȡȻ�󽻸�ondraw()��������û��ȡ�ͽ���ondraw�ᱨ���
	bool m_getfirstPicFlag;
	afx_msg void OnCameracontrolTogglecameracontroldialog();
	//ȷ�� ���˻�ȡ������ť֮�󣬴��߳� ͼ����Ŀ��أ�û�� �̲߳��������߳̾ʹ���
	bool m_isBackgroundImageFlag;
	//ȷ�� ���˻�ȡ������ť֮�󣬳��� �����̴߳������ondraw��ʾ������ ���� ��ondraw��ʾ Ȼ�����̴߳��� �ͱ���
	bool m_isBackgroundImageOperateFlag;
	//vector<CPoint> pointArray;
	bool GetProjectImage(void);
	unsigned char * GetProjectedPixels(void);
	CString GetHasClickedKey(void);

	void DoHandleCaptureBackground(WPARAM wParam,LPARAM lParam);
	unsigned char * GetBackgroundPixels(void);
	unsigned char * GetAfterBackgroundMinusImage(void);
	CString m_front_background_Pixelstatics;
};
