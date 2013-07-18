
// FlyCapDoc.cpp : implementation of the CFlyCapDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "FlyCap.h"
#endif

#include "FlyCapDoc.h"
#include "PicAlgorithm.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//.................del........................
// #include <iostream>
// #include <fstream>
// using namespace std;
// // CFlyCapDoc

IMPLEMENT_DYNCREATE(CFlyCapDoc, CDocument)

BEGIN_MESSAGE_MAP(CFlyCapDoc, CDocument)
	ON_COMMAND(ID_CAMERACONTROL_TOGGLECAMERACONTROLDIALOG, &CFlyCapDoc::OnCameracontrolTogglecameracontroldialog)

END_MESSAGE_MAP()


// CFlyCapDoc construction/destruction

CFlyCapDoc::CFlyCapDoc()
	: shownumbers(NULL)
	, m_getfirstPicFlag(false)
	, m_isBackgroundImageFlag(false)
	, m_isBackgroundImageOperateFlag(false)
	, m_front_background_Pixelstatics(_T(""))
{
	// TODO: add one-time construction code here
	InitBitmapStruct( _DEFAULT_WINDOW_X, _DEFAULT_WINDOW_Y );
	m_grabLoopThreadFlag=false;
//	m_pCamera=NULL;
	m_camera=new CCameraDaoImpl;
	m_heventThreadDone=CreateEvent( NULL, FALSE, FALSE, NULL );
	m_grabLoopThread=NULL;
	shownumbers=NULL;
	m_sensorflag=true;
//	pointArray.empty();
	m_imageDao=new CImageDaoImpl;
	m_keylocationDao=new CFindKeyPositionDaoImpl;
	m_projectImage=NULL;
	m_projectBackupImage=NULL;
	m_keypositionVector=NULL;
}

CFlyCapDoc::~CFlyCapDoc()
{
	CloseHandle(m_heventThreadDone);
	m_camera->~CCameraDao();
}

BOOL CFlyCapDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	CWinApp* theApp = AfxGetApp();
	CWnd* mainWnd;
	if( theApp )
	{
		mainWnd = theApp->m_pMainWnd;
		if( mainWnd )
		{
			mainWnd->ShowWindow(SW_HIDE);
		}
	}
	// Set the default image processing parameters
	Image::SetDefaultColorProcessing( NEAREST_NEIGHBOR );
	Image::SetDefaultOutputFormat( PIXEL_FORMAT_BGR );				//改变了  Image::SetDefaultOutputFormat( PIXEL_FORMAT_BGRU );	 
	// If entering this function from File->New Camera, stop the grab thread
	// first before doing anything else
	if (true==m_grabLoopThreadFlag)
	{
		m_grabLoopThreadFlag=false;


		DWORD dwRet = WaitForSingleObject( m_heventThreadDone, 5000 );
		if ( dwRet == WAIT_TIMEOUT )
		{
			// Timed out while waiting for thread to exit
		}


		//m_camCtlDlg.Hide();
		m_camera->CameracontrolToggleDialogHide();
		//m_camCtlDlg.Disconnect();
		m_camera->CameracontrolToggleDialogDisconnect();
		//m_pCamera->Disconnect();
		m_camera->Disconnect();
	}

	// Let the user select a camera
												//....cancel later
// 	bool okSelected;
// 	PGRGuid arGuid[64];
// 	unsigned int size = 64;
// 	CameraSelectionDlg camSlnDlg;
// 	camSlnDlg.ShowModal( &okSelected, arGuid, &size );
// 	if ( okSelected != true )
// 	{
// 		if (false==m_camera->SelectCamera())
// 		{
// 			OnCloseDocument();
// 		}
// 		return FALSE;
// 	}
	if (false==m_camera->SelectCamera())
	{
		OnCloseDocument();
		exit(0);
		return false;
	}
	//InitCamera();
	   
// 	BusManager busMgr;
// 	InterfaceType ifType;
// 	error = busMgr.GetInterfaceTypeFromGuid( &arGuid[0], &ifType );
// 	if ( error != PGRERROR_OK )
// 	{   
// 		return FALSE;
// 	}    
// 
// 	if ( ifType == INTERFACE_GIGE )
// 	{
// 		//m_pCamera = new GigECamera;
// 	}
// 	else
// 	{
// 		m_pCamera = new Camera;
// 	}
// 	// Connect to the 0th selected camera
// 	error = m_pCamera->Connect( &arGuid[0] );
// 	if( error != PGRERROR_OK )
// 	{
// 		CString csMessage;
// 		csMessage.Format(
// 			"Connect Failure: %s", error.GetDescription() );
// 		AfxMessageBox( csMessage, MB_ICONSTOP );
// 
// 		return FALSE;
// 	}
	m_camera->InitCamera();
// 	error = m_pCamera->GetCameraInfo( &m_cameraInfo );					..........
// 	if( error != PGRERROR_OK )
// 	{
// 		CString csMessage;
// 		csMessage.Format(
// 			"CameraInfo Failure: %s", error.GetDescription() );
// 		AfxMessageBox( csMessage, MB_ICONSTOP );
// 
// 		return FALSE;
// 	}
	//CheckFormat7InfoAndSetCustom();
	// Connect the camera control dialog to the camera object
	//	m_camCtlDlg.Connect( m_pCamera );
	m_camera->CameracontrolToggleDialogConnect();

	//read from DBI the projectImage
	//get the project image
	GetProjectImage();

	//Start the grab thread
	m_grabLoopThreadFlag=true;
	AfxBeginThread(CapturePicThreadProc,this);

	return TRUE;
}




// CFlyCapDoc serialization

void CFlyCapDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CFlyCapDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CFlyCapDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CFlyCapDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CFlyCapDoc diagnostics

#ifdef _DEBUG
void CFlyCapDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFlyCapDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CFlyCapDoc commands


void CFlyCapDoc::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_grabLoopThreadFlag = false;      

	DWORD dwRet = WaitForSingleObject( m_heventThreadDone, 5000 );
	if ( dwRet == WAIT_TIMEOUT )
	{
		// Timed out while waiting for thread to exit
	}

		//m_camCtlDlg.Hide();
	m_camera->CameracontrolToggleDialogHide();
	m_camera->CameracontrolToggleDialogDisconnect();
//	m_camCtlDlg.Disconnect();

	if (false==m_camera->isEmpty())
	{
		m_camera->Disconnect();
		//m_pCamera->Disconnect();
	}
	if (NULL!=m_projectImage)
	{
		m_projectImage->~Image();
		m_projectImage=NULL;
	}
	if (NULL!=m_projectBackupImage)
	{
		m_projectBackupImage->~Image();
		m_projectBackupImage=NULL;
	}
	CDocument::OnCloseDocument();
}


// CFlyCapDoc 命令




//void CFlyCapDoc::PrintBuildInfo(void)
//{
//	FC2Version fc2Version;
//	Utilities::GetLibraryVersion( &fc2Version );
//	char version[128];
//	sprintf( 
//		version, 
//		"FlyCapture2 library version: %d.%d.%d.%d\n", 
//		fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );
//
//	printf( "%s", version );
//
//	char timeStamp[512];
//	sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );
//
//	printf( "%s", timeStamp );
//
//}


//void CFlyCapDoc::PrintCameraInfo(CameraInfo *pCamInfo)
//{
//	printf(
//		"\n*** CAMERA INFORMATION ***\n"
//		"Serial number - %u\n"
//		"Camera model - %s\n"
//		"Camera vendor - %s\n"
//		"Sensor - %s\n"
//		"Resolution - %s\n"
//		"Firmware version - %s\n"
//		"Firmware build time - %s\n\n",
//		pCamInfo->serialNumber,
//		pCamInfo->modelName,
//		pCamInfo->vendorName,
//		pCamInfo->sensorInfo,
//		pCamInfo->sensorResolution,
//		pCamInfo->firmwareVersion,
//		pCamInfo->firmwareBuildTime );
//}


void CFlyCapDoc::PrintError(Error error)
{
		error.PrintErrorTrace();
}


//bool CFlyCapDoc::IsDataFlashSupported(void)
//{
// 	const unsigned int k_dataFlashReg = 0x1240;
// 	//Error error;
// 	unsigned int regVal;
// 	error = m_pCamera->ReadRegister( k_dataFlashReg, &regVal );
// 	if (error != PGRERROR_OK)
// 	{
// 		PrintError( error );
// 		return false;
// 	}
// 
// 	return (regVal & 0x80000000) == 0 ? false : true;
// 	return false;
//}



//void CFlyCapDoc::PrintFormat7Capabilities(Format7Info fmt7Info)
//{
// 	printf(
// 		"Max image pixels: (%u, %u)\n"
// 		"Image Unit size: (%u, %u)\n"
// 		"Offset Unit size: (%u, %u)\n"
// 		"Pixel format bitfield: 0x%08x\n",
// 		fmt7Info.maxWidth,
// 		fmt7Info.maxHeight,
// 		fmt7Info.imageHStepSize,
// 		fmt7Info.imageVStepSize,
// 		fmt7Info.offsetHStepSize,
// 		fmt7Info.offsetVStepSize,
// 		fmt7Info.pixelFormatBitField );
//}


//bool CFlyCapDoc::CheckFormat7InfoAndSetCustom(void)
//{
//	// Query for available Format 7 modes
// 	const Mode k_fmt7Mode = m_format7mode;
// 	const PixelFormat k_fmt7PixFmt = m_format7pixel;
// 	Format7Info fmt7Info;
// 	bool supported;
// 	fmt7Info.mode = k_fmt7Mode;
// 	error = m_pCamera->GetFormat7Info( &fmt7Info, &supported );
// 	if (error != PGRERROR_OK)
// 	{
// 		PrintError( error );
// 		return false;
// 	}
// 
// 	PrintFormat7Capabilities( fmt7Info );
// 	if ( (k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0 )
// 	{
// 		// Pixel format not supported!
// 		printf("Pixel format is not supported\n");
// 		return false;
// 	}
// 	// .........setting    parameter....................
// 	Format7ImageSettings fmt7ImageSettings;
// 	fmt7ImageSettings.mode = k_fmt7Mode;
// 	fmt7ImageSettings.offsetX = 0;
// 	fmt7ImageSettings.offsetY = 0;
// 	fmt7ImageSettings.width = fmt7Info.maxWidth;  // ......
// 	fmt7ImageSettings.height =fmt7Info.maxHeight;  // .......
// 	fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;
// 	//.........setting parameter............
// 	bool valid;
// 	Format7PacketInfo fmt7PacketInfo;
// 
// 	// Validate the settings to make sure that they are valid
// 	error = m_pCamera->ValidateFormat7Settings(
// 		&fmt7ImageSettings,
// 		&valid,
// 		&fmt7PacketInfo );
// 	if (error != PGRERROR_OK)
// 	{
// 		PrintError( error );
// 		return false;
// 	}
// 
// 	if ( !valid )
// 	{
// 		// Settings are not valid
// 		printf("Format7 settings are not valid\n");
// 		return false;
// 	}
// 	// Set the settings to the camera
// 	error = m_pCamera->SetFormat7Configuration(
// 		&fmt7ImageSettings,
// 		fmt7PacketInfo.recommendedBytesPerPacket );
// 	if (error != PGRERROR_OK)
// 	{
// 		PrintError( error );
// 		return false;
// 	}
// 
// 	return true;
//}

//void CFlyCapDoc::PrintFrameCameraRate()
//{
// 	Retrieve frame rate property
// 	Property frmRate;
// 	frmRate.type = FRAME_RATE;
// 	error = m_pCamera->GetProperty( &frmRate );
// 	if (error != PGRERROR_OK)
// 	{
// 		PrintError( error );
// 		printf("FAIL TO CHECK OUT THE FRAME_RATE PROPERTY");
// 		return ;
// 	}
// 	printf( "Frame rate is %3.2f fps\n", frmRate.absValue );
//
//}





//bool CFlyCapDoc::InitCamera(void)			//.........................
//{
//	
//
//}


bool CFlyCapDoc::CaptureBackgroundImagePreProcess(void)
{
	//error = m_pCamera->RetrieveBuffer( &m_backgroundImage );
	//CFlyCapView *pView=(CFlyCapView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
	


	//这里应该发个重画消息  和发送 获取背景消息 由相应的获取背景函数来 获取

	//重新获取背景的时候确保 之前已按的键消失 然后采集背景
	//int x=m_imageDao->GetHasClickedKey().GetLength();

	//先关闭线程处理,   此时线程处理获取图片的操作还是存在的
	m_isBackgroundImageFlag=false;
	m_isBackgroundImageOperateFlag=false;
	if (0!=m_imageDao->GetHasClickedKey().GetLength())
	{	
		//m_imageDao->ClearHasClickedKey();
		RedrawAllViews();
		//Sleep(5000);
	}
	
	
	return true;
}


unsigned char* CFlyCapDoc::GetProcessedPixels(void)
{
	if (m_processedImage.GetDataSize()==0)
	{
		return NULL;
	}
// 	unsigned char *p=m_processedImage.GetData();
// 	unsigned int len=m_processedImage.GetDataSize();
// 	char *ch;
// 	
// 
// 	ofstream of("test.txt");
// 	
// 	of.write((char*)p,len);
// 	
		
	return m_processedImage.GetData();
}


bool CFlyCapDoc::saveCaptureImageToDisk(void)
{
	CFileDialog savedialog(FALSE );
	savedialog.m_ofn.lpstrTitle="我的文件保存对话框";
	savedialog.m_ofn.lpstrFilter="所有文件(*.*)\0*.*\0JPG Files(*.jpg)\0*.jpg\0JPEG Files(*.jpeg)\0*.jpeg\0BMP Files(*.bmp)\0*.bmp\PNG Files(*.png)\0PNG Files(*.png)\0*.png\0\0"; 
	savedialog.m_ofn.lpstrDefExt="jpeg";

	if(savedialog.DoModal()==IDOK )
	{
		Image convertedImage;
		error = m_rawImage.Convert( PIXEL_FORMAT_BGR, &convertedImage );
		if (error!=PGRERROR_OK)
		{
			PrintError( error );
			return false;
		} 
		CString filepathname=savedialog.GetPathName();


		CString availablefilename="";
		for (int i=0;i<filepathname.GetLength();i++)
		{
			if (filepathname.GetAt(i)!='\\')
			{
				availablefilename.AppendChar(filepathname.GetAt(i));
			}
			else
			{
				availablefilename.AppendChar('\\');
				availablefilename.AppendChar('\\');
			}
		}
		error = convertedImage.Save( availablefilename );
		//error=m_rawImage.Save(savedialog.GetFileName());

		if (error!=PGRERROR_OK)
		{
			PrintError( error );
			return false;
		}
	}
	return true;
}


//bool CFlyCapDoc::StopAndDisconnectCamera(void)
//{
// 	Stop the Camera
// 		error = m_pCamera->StopCapture();
// 		if (error != PGRERROR_OK)
// 		{
// 			PrintError( error );
// 			return false;
// 		}      
// 		// Disconnect the camera
// 		error = m_pCamera->Disconnect();
// 		if (error != PGRERROR_OK)
// 		{
// 			PrintError( error );
// 			return false;
// 		}
// 	
// 		return true;
//}

//bool CFlyCapDoc::StartCamera(void)
//{
//	
// 	error = m_pCamera->StartCapture();
// 	if (error != PGRERROR_OK)
// 	{
// 		PrintError( error );
// 		return false;
// 	}
// 	return true;
//}

bool CFlyCapDoc::StartCapturePicThread(void)
{
	m_grabLoopThreadFlag=true;

	m_grabLoopThread=AfxBeginThread( CapturePicThreadProc, this );
	TRACE("线程已启动");
	return true;
}



bool CFlyCapDoc::StopCapturePicThread(void)
{
	m_grabLoopThreadFlag=false;
	DWORD dwRet =WaitForSingleObject(m_grabLoopThread->m_hThread,5000);
	if (dwRet==WAIT_TIMEOUT)
	{
		TerminateThread(m_grabLoopThread->m_hThread,0);
	}
	return true;
}


bool CFlyCapDoc::SetFrameRate(double framerate)
{
	//m_pCamera->StartSyncCapture()
	m_processedFrameRate.SetFrameRate(framerate);
	return true;
}


bool CFlyCapDoc::IsGrabThreadRunning(void)
{
	return m_grabLoopThreadFlag;
}





void CFlyCapDoc::SaveAviHelper(	std::vector<Image>& vecImages, std::string aviFileName, float frameRate)
{
	Error error;
	AVIRecorder aviRecorder;

	// Open the AVI file for appending images
	/*
	switch (UNCOMPRESSED)
	{
	case UNCOMPRESSED:
		{
			AVIOption option;     
			option.frameRate = frameRate;
			error = aviRecorder.AVIOpen(aviFileName.c_str(), &option);
		}
		break;
	case MJPG:
		{
			MJPGOption option;
			option.frameRate = frameRate;
			option.quality = 75;
			error = aviRecorder.AVIOpen(aviFileName.c_str(), &option);
		}
		break;
	case H264:
		{
			H264Option option;
			option.frameRate = frameRate;
			option.bitrate = 1000000;
			option.height = vecImages[0].GetRows();
			option.width = vecImages[0].GetCols();
			error = aviRecorder.AVIOpen(aviFileName.c_str(), &option);
		}
		break;
	}       
	*/
	AVIOption option;     
	option.frameRate = frameRate;
	error = aviRecorder.AVIOpen(aviFileName.c_str(), &option);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return;
	}    

	printf( "\nAppending %d images to AVI file: %s ... \n", vecImages.size(), aviFileName.c_str() ); 
	for (int imageCnt = 0; imageCnt < vecImages.size(); imageCnt++)    
	{
		// Append the image to AVI file
		error = aviRecorder.AVIAppend(&vecImages[imageCnt]);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			continue;
		}

		printf("Appended image %d...\n", imageCnt); 
	}

	// Close the AVI file
	error = aviRecorder.AVIClose( );
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return;
	} 
}


UINT CFlyCapDoc::CapturePicThreadProc(LPVOID pparam)
{

	CFlyCapDoc *helper=(CFlyCapDoc *)pparam;
	Error error;
	UINT uiRetval=helper->DoGrabLoop();
	if( uiRetval != 0 )
	{
		CString csMessage;
		csMessage.Format(
			"The grab thread has encountered a problem and had to terminate." );
		AfxMessageBox( csMessage, MB_ICONSTOP );

		//
		// Signal that the thread has died.
		//
		SetEvent( helper->m_heventThreadDone );   //.....调用其他线程 一个 
	}
	return uiRetval;
	/*
	static int num=0;	//图片编号
	while(true==helper->m_grabLoopThreadFlag)
	{
		error=helper->m_pCamera->RetrieveBuffer(&rawImage);
		if (error != PGRERROR_OK)
		{
			PrintError( error );
			return false;
		}
		Image convertedImage;
		error = rawImage.Convert( PIXEL_FORMAT_BGRU, &convertedImage );
		if (error!=PGRERROR_OK)
		{
			PrintError( error );
			return false;
		} 
		// deal with
		CString filename;
		filename.Format("%d.jpeg",num);
		num++;
		convertedImage.Save(filename);
		if (error!=PGRERROR_OK)
		{
			PrintError( error );
			return false;
		} 
		//deal with the picture m_rawImage;
		//
		//
	}
	//const UINT uiRetval=0;
	*/
	//return 0;
}


UINT CFlyCapDoc::DoGrabLoop(void)
{
	CString csMessage;
	unsigned char *deldata;
	//error = m_pCamera->StartCapture();
	m_camera->StartCapture();
// 	if( error != PGRERROR_OK )
// 	{
// 		csMessage.Format(
// 			"StartCapture Failure: %s", error.GetDescription() );
// 		AfxMessageBox( csMessage, MB_ICONSTOP );
// 		return 0;
// 	}
	//
	// Start of main grab loop
	//
	while(m_grabLoopThreadFlag)
	{
		/************************************************************************/
		/*  the first      get the raw Image                                                               */
		/************************************************************************/
		bool flag=m_camera->RetrieveBuffer( &m_rawImage );
	//	m_rawImage.Save("raw.bmp");
		if( false==flag )
		{
			csMessage.Format(
				"RetrieveBuffer Failure: %s", error.GetDescription() );

			continue;
		}


		//
		// Check to see if the thread should die.
		//
		if( !m_grabLoopThreadFlag )
		{
			break;
		}
		//
		// Update current framerate.
		//
		m_processedFrameRate.NewFrame();
		  
		//
		// We try to detect whether the view is getting behind on servicing
		// the invalidate requests we send to it.  If there is still an 
		// invalid area, don't bother color processing this frame.
		//
		bool skipProcessing = false;

		POSITION pos = GetFirstViewPosition();
		while ( pos != NULL )
		{
			if( GetUpdateRect( GetNextView( pos )->GetSafeHwnd(), NULL, FALSE ) != 0 )
			{
				skipProcessing = true;
			}
		} 

		if( !skipProcessing )
		{
			//
			// Do post processing on the image.
			//
			unsigned int rows,cols,stride;
			PixelFormat format;
			m_rawImage.GetDimensions( &rows, &cols, &stride, &format );    

			CSingleLock dataLock( &m_csData );
			dataLock.Lock();

			m_getfirstPicFlag=true;
			error=m_rawImage.Convert(PIXEL_FORMAT_BGR,&m_processedImage);
			//m_imageDao->DeepCopy(&m_processedImage);							//拷贝一份处理    //m_processedImage used for the last compare
			
			
			if(true==m_isBackgroundImageFlag)
			{
				//说明背景处理按钮按了之后，第一次针对背景图片处理
				//if(false==m_isBackgroundImageOperateFlag)
				// do something
				m_isBackgroundImageOperateFlag=true;	
				//m_imageDao->RGBtoGray();
				//m_imageDao->ConvertGray(&m_processedImage);
				//m_rawImage.Convert(PIXEL_FORMAT_MONO8,m_imageDao->GetIma ge());
				m_imageDao->ConvertGray(&m_processedImage);
				m_imageDao->SaveImage("m_imageDao.bmp");
				m_grayImage.DeepCopy(m_imageDao->GetImage());

				m_imageDao->GetGrayAndBackgroundPTR(m_grayImage.GetData(),m_backgroundImage.GetData());

				if(false==m_imageDao->ImageMinus(&m_backgroundImage))
				{
					AfxMessageBox("请重新获取背景");
					m_isBackgroundImageFlag=false;
					m_isBackgroundImageOperateFlag=false;
				}
				else
				{
					m_imageDao->SaveImage("backminus.bmp");
					//	m_imageDao->AdaptiveMedianFilter(3,1,0,1);					中值改进滤波
					m_afterBackgroundMinusImage.DeepCopy(m_imageDao->GetImage());
 					m_imageDao->ConvertGray2BW();
					//after 2B, we can get the background and front PixelStatics
					m_front_background_Pixelstatics=m_imageDao->Getfront_background_Pixelstatics();
					m_imageDao->SaveImage("grayto2B.bmp");
					//int se[3][3]={{-1,1,-1},{1,1,1},{-1,1,-1}};
					//m_imageDao->Close(se);
					//m_imageDao->Open(se);
				//	m_imageDao->SearchFingerPoint(m_processedImage.GetData(),3,3,1,1,m_keypositionVector);
					m_imageDao->SearchFingerPoint(m_processedImage.GetData(),5,5,2,2,m_keypositionVector);
					m_2BImage.DeepCopy(m_imageDao->GetImage());
					//找指尖
					m_showImage.DeepCopy(&m_processedImage);
					m_imageDao->SetImagePointTrace(m_showImage.GetData());
					m_showImage.Save("showimage.bmp");
					m_imageDao->BackgroundUpdateOperate(m_grayImage.GetData(),m_backgroundImage.GetData());
					m_backgroundImage.Save("background.bmp");
				}
			}
			

			//////////////////////////////////////////////////////////////////////////
		
			/************************************************************************/
			/* the second  gray Image                                                                     */
			/************************************************************************/
// 			PUCHAR data=m_picalgorithm.RGB2Gray(m_processedImage.GetData(),m_processedImage.GetCols(),m_processedImage.GetRows(),24);		
// 			unsigned char *aa=m_grayImage.GetData();
// 			error=m_grayImage.ReleaseBuffer();
// 			unsigned char *ff=m_grayImage.GetData();
// 			delete []aa;
// 			m_grayImage.SetData(data,m_grayImage.GetDataSize()/3);
// 			m_grayImage.Save("gray.bmp");
// 			
// 			
// 
// 			/************************************************************************/
// 			/* third   2B Image                                                                     */
// 			/************************************************************************/
// 			
// 			PUCHAR data2B=m_picalgorithm.Gray2BW(data,m_processedImage.GetCols(),m_processedImage.GetRows());
// 			unsigned char *deldata=m_2BImage.GetData();
// 			delete [] deldata;
// 			deldata=NULL;
// 			m_2BImage.ReleaseBuffer();
// 			m_2BImage.SetData(data2B,m_processedImage.GetDataSize()/3);
// 			int x=m_picalgorithm.getRect(data2B,m_processedImage.GetCols(),m_processedImage.GetRows(),200,200,100,100);
// 			if (m_sensorflag==true&&x==1)
// 			{
// 				AfxMessageBox("x=1");
// 			}
// 			if(x==0)
// 			{
// 				m_sensorflag=true;
// 			}
// 			else
// 			{
// 				m_sensorflag=false;
// 			}
// 
// 			/************************************************************************/
// 			/*  fourth     deal with                                                                      */
// 			/************************************************************************/
// 
// 			PUCHAR dataDraw=m_picalgorithm.drawEmptyRect(data,m_processedImage.GetCols(),m_processedImage.GetRows(),200,200,100,100,10);							
// 			
// 			unsigned char * del=m_showImage.GetData();
// 			delete []del;
// 			del=NULL;
// 			m_showImage.ReleaseBuffer();
// 			m_showImage.SetData(dataDraw,m_showImage.GetDataSize()/3);
// 			m_showImage.Convert(PIXEL_FORMAT_MONO8,&m_showImage);
			//////////////////////////////////////////////////////////////////////////
			dataLock.Unlock();

			InitBitmapStruct( cols, rows ); 

			RedrawAllViews(); 
		}
	}  

	// extra code for test algorithm


	//error = m_pCamera->StopCapture();
	m_camera->StopCapture();
// 	if( error != PGRERROR_OK )
// 	{
// 		csMessage.Format(
// 			"Stop Failure: %s", error.GetDescription() );
// 		AfxMessageBox( csMessage, MB_ICONSTOP );
// 	}

	//
	// End of main grab loop
	//
	SetEvent( m_heventThreadDone );

	return 0;

}


void CFlyCapDoc::RedrawAllViews(void)
{
	POSITION pos = GetFirstViewPosition();
	while ( pos != NULL )
	{
		InvalidateRect( GetNextView( pos )->GetSafeHwnd(), NULL, FALSE );
	}    
}


void CFlyCapDoc::InitBitmapStruct(int cols, int rows)
{
	BITMAPINFOHEADER* pheader = &m_bitmapInfo.bmiHeader;

	// Initialize permanent data in the bitmapinfo header.
	pheader->biSize          = sizeof( BITMAPINFOHEADER );
	pheader->biPlanes        = 1;
	pheader->biCompression   = BI_RGB;
	pheader->biXPelsPerMeter = 100;
	pheader->biYPelsPerMeter = 100;
	pheader->biClrUsed       = 0;
	pheader->biClrImportant  = 0;

	// Set a default window size.
	pheader->biWidth    = cols;
	pheader->biHeight   = -rows;
	pheader->biBitCount = 32;

	m_bitmapInfo.bmiHeader.biSizeImage = 0;
}


double CFlyCapDoc::GetProcessedFrameRate(void)
{

	return m_processedFrameRate.GetFrameRate();
}

void CFlyCapDoc::GetImageSize( unsigned int* pWidth, unsigned int* pHeight )
{
	//m_rawImage.GetDimensions( puiHeight, puiWidth );
	*pWidth = abs(m_bitmapInfo.bmiHeader.biWidth);
	*pHeight = abs(m_bitmapInfo.bmiHeader.biHeight);
}

unsigned char * CFlyCapDoc::GetGRAYPixels(void)
{
	return m_grayImage.GetData();;
}


unsigned char * CFlyCapDoc::Get2BImagePixels(void)
{
	return m_2BImage.GetData();
}


unsigned char * CFlyCapDoc::getShowPixels(void)
{
	return m_showImage.GetData();
}




void CFlyCapDoc::OnCameracontrolTogglecameracontroldialog()
{
	// TODO: 在此添加命令处理程序代码
	if (true==m_camera->IsCameracontrolToggleDialogVisible())
	{
		m_camera->CameracontrolToggleDialogHide();
	}
	else
	{
		m_camera->CameracontrolToggleDialogShow();
	}
}




bool CFlyCapDoc::GetProjectImage(void)
{
// 	CBitmap bitmap;
// 	bitmap.LoadBitmapA(IDB_BITMAP_KEYBOARD);
// 
// 	BITMAP bmp;
// 	
// 	bitmap.GetBitmap(&bmp);
// 
// 	unsigned char *p_Imagedata=new unsigned char[bmp.bmWidthBytes*bmp.bmHeight];
// 	DWORD imageCount=bmp.bmWidthBytes*bmp.bmHeight;
// 
// 	bitmap.GetBitmapBits(imageCount,p_Imagedata);
// 
// 	CImage imgTemp;
// 	imgTemp.Attach(bitmap.operator HBITMAP());
// 	imgTemp.Save("wfwfwProject.bmp");
// 
// 	CImage wfImg;
// 	wfImg.LoadFromResource(theApp.m_hInstance,IDB_BITMAP_KEYBOARD);
// 	unsigned char * data=(unsigned char *)wfImg.GetBits();
// 	
// 	wfImg.Save("CIMAG.bmp");
// 	m_projectImage=new Image(bmp.bmHeight,bmp.bmWidth,PIXEL_FORMAT_RGBU);
// 	m_projectImage->SetData(p_Imagedata,bmp.bmHeight*bmp.bmWidthBytes);
// 	
// 
// 	m_projectImage->Save("projectImageBeforeConvert.bmp");
// 	m_projectImage->Convert(PIXEL_FORMAT_BGR,m_projectImage);
// 
// 	m_projectBackupImage=new Image;
// 
// 	m_projectBackupImage->DeepCopy(m_projectImage);
// 
// 	m_projectImage->Save("projectImage.bmp");
// 	m_projectBackupImage->Save("projectBackupImage.bmp");


	CImage image;
	image.LoadFromResource(theApp.m_hInstance,IDB_BITMAP_KEYBOARD);
	unsigned char *data=(unsigned char *)image.GetBits();
	image.Save("fffff.bmp");

	int nwidth,nheight,npatch;
	nwidth=image.GetWidth();
	nheight=image.GetHeight();
	npatch=image.GetPitch();
	unsigned char *mydata=new unsigned char[nwidth*nheight*3];
	for (int i=0;i<nheight;i++)
	{
		for(int j=0;j<abs(npatch);j++)
		
			{
				*(mydata+i*nwidth*3+j)=*(data+i*npatch+j);
			}
	}
	
	m_projectImage=new Image(nheight,nwidth,PIXEL_FORMAT_BGR);
	error=m_projectImage->SetData(mydata,nwidth*nheight*3);
	m_projectBackupImage=new Image(nheight,nwidth,PIXEL_FORMAT_BGR);
	m_projectBackupImage->DeepCopy(m_projectImage);



	m_projectImage->Save("projectImage.bmp");
	m_projectBackupImage->Save("projectBackupImage.bmp");
	for (int i=0;i<nheight;i++)
	{
		for(int j=0;j<nwidth;j++)
		{
			if (mydata[i*nwidth*3+j*3]==255&&mydata[i*nwidth*3+j*3+1]==255&&mydata[i*nwidth*3+j*3+2]==255)
			{
				m_projectImageSeparateLocationy=i;
				return true;
			}
		}
	}
	return true;

}


unsigned char * CFlyCapDoc::GetProjectedPixels(void)
{
	if (m_projectBackupImage==NULL)
	{
		return NULL;
	}
	m_projectImage->DeepCopy(m_projectBackupImage);
	return m_projectImage->GetData();
}


CString CFlyCapDoc::GetHasClickedKey(void)
{
	m_keyClickedboard.Empty();

	m_keyClickedboard=m_imageDao->GetHasClickedKey();
	
	

	return m_keyClickedboard;
}


void CFlyCapDoc::DoHandleCaptureBackground(WPARAM wParam,LPARAM lParam)
{
	Sleep(1000);
	m_processedImage.Convert(PIXEL_FORMAT_MONO8,&m_backgroundImage);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		//return false;
	}

	m_backgroundImage.Save("background.bmp");

	int view_keyBoardFrameFlag=(int)lParam;
	if (false==view_keyBoardFrameFlag)
	{
		m_isBackgroundImageFlag=true;
		m_isBackgroundImageOperateFlag=false;
		return;
	}
	/// 
	// 调试
	//找 区域位置
	if(false==m_keylocationDao->RegionGrow(m_backgroundImage.GetData(),m_backgroundImage.GetCols(),m_backgroundImage.GetRows()))
	{
		m_isBackgroundImageFlag=false;
		m_isBackgroundImageOperateFlag=false;
		return;
	}
	m_keypositionVector=m_keylocationDao->getLocationPosition();
	//////////////////////////////////////////////////////////////////////////
	
	
//	m_imageDao->ResetCountImageUpdatePixels();
	//打开线程处理的操作  要最后
	m_isBackgroundImageFlag=true;
	//是要去掉的m_isBackgroundImageOperateFlag=true;
	//return 0;
}


unsigned char * CFlyCapDoc::GetBackgroundPixels(void)
{
	if (m_backgroundImage.GetDataSize()==0)
	{
		return NULL;
	}
	return m_backgroundImage.GetData();
}


unsigned char * CFlyCapDoc::GetAfterBackgroundMinusImage(void)
{
	if (m_afterBackgroundMinusImage.GetDataSize()==0)
	{
		return NULL;
	}
	return m_afterBackgroundMinusImage.GetData();
}
