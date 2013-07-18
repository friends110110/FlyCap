#include "StdAfx.h"
#include "CameraDaoImpl.h"


CCameraDaoImpl::CCameraDaoImpl(void)
{
	m_pCamera=new Camera;
	m_camCtlDlg=new CameraControlDlg;
	m_cameraInfo=new CameraInfo;
}


CCameraDaoImpl::~CCameraDaoImpl(void)
{
	if (m_pCamera!=NULL)
	{
		delete m_pCamera;
	}
	if(m_camCtlDlg!=NULL)
	{
		delete m_camCtlDlg;
	}
	if (m_cameraInfo!=NULL)
	{
		delete m_cameraInfo;
	}
}

void CCameraDaoImpl::PrintError(Error error)
{
	error.PrintErrorTrace();
}
bool CCameraDaoImpl::InitCamera()
{
	BusManager busMgr;
	unsigned int numCameras;
	error = busMgr.GetNumOfCameras(&numCameras);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return false;
	}

	printf( "Number of cameras detected: %u\n", numCameras );

	if ( numCameras < 1 )
	{
		printf( "Insufficient number of cameras... exiting\n" );
		return false;
	}
	PGRGuid guid;
	error = busMgr.GetCameraFromIndex(0, &guid);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return false;
	}
	// Connect to a camera
	error = m_pCamera->Connect(&guid);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return false;
	}
	// Get the camera information
	CameraInfo camInfo;
	error = m_pCamera->GetCameraInfo(&camInfo);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}

	this->PrintCameraInfo(&camInfo);  

	//CheckFormat7InfoAndSetCustom();
	return true;

// 	BusManager busMgr;
// 	InterfaceType ifType;
// 	PGRGuid arGuid[64];
// 	error = busMgr.GetInterfaceTypeFromGuid( &arGuid[0], &ifType );
// 	if ( error != PGRERROR_OK )
// 	{   
// 		return FALSE;
// 	}    
// 
// 	if ( ifType == INTERFACE_GIGE )
// 	{
// 	//	m_pCamera = new GigECamera;
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
// 		return false;
// 	}
// 	this->CameracontrolToggleDialogConnect();
// 	return true;
}
bool CCameraDaoImpl::isEmpty()		//is exist
{
	if (m_pCamera==NULL)
	{
		return true;
	}
	return false;
}
bool CCameraDaoImpl::DelCamera()
{
	if (m_pCamera!=NULL)
	{
		delete m_pCamera;
	}
	return true;
}

bool CCameraDaoImpl::SelectCamera()
{
	// Let the user select a camera
	//....cancel later
	bool okSelected;
	PGRGuid arGuid[64];
	unsigned int size = 64;
	CameraSelectionDlg camSlnDlg;
	camSlnDlg.ShowModal( &okSelected, arGuid, &size );
	if ( okSelected != true )
	{
		//if (m_pCamera != NULL)
		{
			return false;
		}
		
	}
	return true;
 }
  bool CCameraDaoImpl::StartCamera()
{
	error = m_pCamera->StartCapture();
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}
	return true;

}
 bool CCameraDaoImpl::StartCapture()
 {
	 CString csMessage;
	 error=m_pCamera->StartCapture();
	 if( error != PGRERROR_OK )
	 {
		 csMessage.Format(
			 "StartCapture Failure: %s", error.GetDescription() );
		 AfxMessageBox( csMessage, MB_ICONSTOP );
		 return false;
	 }
	 return true;
 }
  bool CCameraDaoImpl::connect(PGRGuid * arGuid)
{
	error=m_pCamera->Connect(arGuid);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}
	return true;
}
  bool CCameraDaoImpl::Disconnect()
{
	error=m_pCamera->Disconnect();
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}
	return true;
}
  bool CCameraDaoImpl::StopCapture()
{
	CString csMessage;
	error=m_pCamera->StopCapture();
	if (error != PGRERROR_OK)
	{
		csMessage.Format(
			 			"Stop Failure: %s", error.GetDescription() );
			 		AfxMessageBox( csMessage, MB_ICONSTOP );
		PrintError( error );
		return false;
	}
	return true;
}
  bool CCameraDaoImpl::StopAndDisconnectCamera()
{
	//Stop the Camera
	error = m_pCamera->StopCapture();
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}      
	// Disconnect the camera
	error = m_pCamera->Disconnect();
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}

	return true;
}
  bool CCameraDaoImpl::RetrieveBuffer(Image * image)
{
	error=m_pCamera->RetrieveBuffer(image);
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}
	return true;
}

  bool CCameraDaoImpl::GetCameraInfo(CameraInfo * m_cameraInfo)
{
	error = m_pCamera->GetCameraInfo( m_cameraInfo );
	if( error != PGRERROR_OK )
	{
		CString csMessage;
		csMessage.Format(
			"CameraInfo Failure: %s", error.GetDescription() );
		AfxMessageBox( csMessage, MB_ICONSTOP );
		return false;
	}
	return true;
}
  bool CCameraDaoImpl::IsDataFlashSupported()
{
	const unsigned int k_dataFlashReg = 0x1240;
	//Error error;
	unsigned int regVal;
	error = m_pCamera->ReadRegister( k_dataFlashReg, &regVal );
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}

	return (regVal & 0x80000000) == 0 ? false : true;
	return false;
}
  bool CCameraDaoImpl::PrintFormat7Capabilities(Format7Info *fmt7Info)
{
	printf(
		"Max image pixels: (%u, %u)\n"
		"Image Unit size: (%u, %u)\n"
		"Offset Unit size: (%u, %u)\n"
		"Pixel format bitfield: 0x%08x\n",
		fmt7Info->maxWidth,
		fmt7Info->maxHeight,
		fmt7Info->imageHStepSize,
		fmt7Info->imageVStepSize,
		fmt7Info->offsetHStepSize,
		fmt7Info->offsetVStepSize,
		fmt7Info->pixelFormatBitField );
	return true;
}
 bool CCameraDaoImpl::CheckFormat7InfoAndSetCustom()
{
	const Mode k_fmt7Mode = m_format7mode;
	const PixelFormat k_fmt7PixFmt = m_format7pixel;
	Format7Info fmt7Info;
	bool supported;
	fmt7Info.mode = k_fmt7Mode;
	error = m_pCamera->GetFormat7Info( &fmt7Info, &supported );
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}

	PrintFormat7Capabilities( &fmt7Info );
	if ( (k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0 )
	{
		// Pixel format not supported!
		printf("Pixel format is not supported\n");
		return false;
	}
	// .........setting    parameter....................
	Format7ImageSettings fmt7ImageSettings;
	fmt7ImageSettings.mode = k_fmt7Mode;
	fmt7ImageSettings.offsetX = 0;
	fmt7ImageSettings.offsetY = 0;
	fmt7ImageSettings.width = fmt7Info.maxWidth;  // ......
	fmt7ImageSettings.height =fmt7Info.maxHeight;  // .......
	fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;
	//.........setting parameter............
	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	// Validate the settings to make sure that they are valid
	error = m_pCamera->ValidateFormat7Settings(
		&fmt7ImageSettings,
		&valid,
		&fmt7PacketInfo );
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}

	if ( !valid )
	{
		// Settings are not valid
		printf("Format7 settings are not valid\n");
		return false;
	}
	// Set the settings to the camera
	error = m_pCamera->SetFormat7Configuration(
		&fmt7ImageSettings,
		fmt7PacketInfo.recommendedBytesPerPacket );
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		return false;
	}

	return true;
}
  bool CCameraDaoImpl::PrintFrameCameraRate()
{
	// Retrieve frame rate property
	Property frmRate;
	frmRate.type = FRAME_RATE;
	error = m_pCamera->GetProperty( &frmRate );
	if (error != PGRERROR_OK)
	{
		PrintError( error );
		printf("FAIL TO CHECK OUT THE FRAME_RATE PROPERTY");
		return false;
	}
	printf( "Frame rate is %3.2f fps\n", frmRate.absValue );
	return true;
}
   bool CCameraDaoImpl::PrintCameraInfo(CameraInfo * pCamInfo)
   {
	   printf(
		   "\n*** CAMERA INFORMATION ***\n"
		   "Serial number - %u\n"
		   "Camera model - %s\n"
		   "Camera vendor - %s\n"
		   "Sensor - %s\n"
		   "Resolution - %s\n"
		   "Firmware version - %s\n"
		   "Firmware build time - %s\n\n",
		   pCamInfo->serialNumber,
		   pCamInfo->modelName,
		   pCamInfo->vendorName,
		   pCamInfo->sensorInfo,
		   pCamInfo->sensorResolution,
		   pCamInfo->firmwareVersion,
		   pCamInfo->firmwareBuildTime );
	   return true;
   }
 bool CCameraDaoImpl::PrintBuildInfo()
 {
	  	FC2Version fc2Version;
	  	Utilities::GetLibraryVersion( &fc2Version );
	  	char version[128];
	  	sprintf( 
	  		version, 
	  		"FlyCapture2 library version: %d.%d.%d.%d\n", 
	  		fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );
	  
	  	printf( "%s", version );
	  
	  	char timeStamp[512];
	  	sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );
	  
	  	printf( "%s", timeStamp );
		return true;
 }

  bool CCameraDaoImpl::CameracontrolToggleDialogHide()
{
	m_camCtlDlg->Hide();
	return true;
}
  bool CCameraDaoImpl::CameracontrolToggleDialogShow()
{
	m_camCtlDlg->Show();
	return true;
}

  bool CCameraDaoImpl::CameracontrolToggleDialogConnect()
  {
	  m_camCtlDlg->Connect(m_pCamera);
	  return true;
  }
   bool CCameraDaoImpl::CameracontrolToggleDialogDisconnect()
   {
	   m_camCtlDlg->Disconnect();
	   return true;
   }

   	bool CCameraDaoImpl::IsCameracontrolToggleDialogVisible()
	{
		if (true==m_camCtlDlg->IsVisible())
		{
			return true;
		}
		
		return false;
	}