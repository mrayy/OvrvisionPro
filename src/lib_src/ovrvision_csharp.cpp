// ovrvision_csharp.cpp
//
//MIT License
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
//
// Oculus Rift : TM & Copyright Oculus VR, Inc. All Rights Reserved
// Unity : TM & Copyright Unity Technologies. All Rights Reserved

/////////// INCLUDE ///////////

#ifdef WIN32
#include <ovrvision_pro.h>
#include <ovrvision_ar.h>
#include <ovrvision_tracking.h>
#include "ovrvision_setting.h"
#include "ovrvision_calibration.h"
#include "UnityGraphicsDevice.h"


#elif MACOSX
//config gdata class
#include "ovrvision_setting.h"
#include "ovrvision_tracking.h"
#include "ovrvision_calibration.h"
#undef _OVRVISION_EXPORTS
#include "ovrvision_pro.h"
#include "ovrvision_ar.h"
#else //LINUX
#include <ovrvision_pro.h>
#include <ovrvision_ar.h>
#include <ovrvision_tracking.h>
#include "ovrvision_setting.h"
#include "ovrvision_calibration.h"
#endif

#ifdef WIN32
#define SUPPORT_D3D9 1
#define SUPPORT_D3D11 1 // comment this out if you don't have D3D11 header/library files
#define SUPPORT_OPENGL 1
#endif

#ifdef MACOSX
#define SUPPORT_OPENGL 1
#endif

#ifdef LINUX
#define SUPPORT_OPENGL 1
#endif


#if SUPPORT_D3D9
#include <d3d9.h>
#endif

#if SUPPORT_D3D11
#include <d3d11.h>
#endif

#include <list>
#include "WinMutex.h"

#if SUPPORT_OPENGL
	#if WIN32
	#include <gl/GL.h>
	#elif MACOSX
	#include <OpenGL/gl.h>
	typedef unsigned int GLuint;
	#else //LINUX
	#include <GL/gl.h>
	#include <GL/glx.h>
	#endif
#endif

/////////// VARS AND DEFS ///////////

//Exporter
#ifdef WIN32
#define CSHARP_EXPORT __declspec(dllexport)
#elif MACOSX
#define CSHARP_EXPORT 
#else //LINUX
#define CSHARP_EXPORT
#endif

//AR deta size
#define FLOATDATA_DATA_OFFSET	(10)

//Size
#define BGR_DATASIZE	(3)


struct GLRenderCall
{
	//for GL Call
	OVR::OvrvisionPro* ovOvrvision;
	void* callTexture2DLeft ;
	void* callTexture2DRight;
};

std::list<GLRenderCall> g_calls;
WinMutex g_mutex;

/////////// EXPORT FUNCTION ///////////

//C language
#ifdef __cplusplus
extern "C" {
#endif

// Provide them with an address to a function of this signature.
typedef void(__stdcall * UnityRenderNative)(int eventID);

CSHARP_EXPORT OVR::OvrvisionPro* ovCreateInstance()
{
	return new OVR::OvrvisionPro();	//MainVideo
}

// int ovOpen(void)
CSHARP_EXPORT int ovOpen(OVR::OvrvisionPro*g_ovOvrvision, int locationID, int type)
{
	//Create object
	if (g_ovOvrvision == NULL)
		return 1;
		

	//Ovrvision Open
	if (g_ovOvrvision->Open(locationID, (OVR::Camprop)type) == 0)	//0=Error
		return 1;	//FALSE


	return 0;	//OK
}
CSHARP_EXPORT int ovOpenMemory(OVR::OvrvisionPro*g_ovOvrvision, float arMeter, int type)
{
	//Create object
	if (g_ovOvrvision == NULL)
		return 1;

	//Ovrvision Open
	if (g_ovOvrvision->OpenMemory((OVR::Camprop)type) == 0)	//0=Error
		return 1;	//FALSE


	return 0;	//OK
}

// int ovClose(void)
CSHARP_EXPORT int ovClose(OVR::OvrvisionPro*g_ovOvrvision)
{

	//Close
	if (g_ovOvrvision)
		g_ovOvrvision->Close();

	return 0;	//OK
}

// int ovRelease(void) -> Exit
CSHARP_EXPORT int ovRelease(OVR::OvrvisionPro*g_ovOvrvision)
{

	if (g_ovOvrvision) {
		delete g_ovOvrvision;
		g_ovOvrvision = NULL;
	}

	return 0;	//OK
}

// int ovPreStoreCamData() -> need ovGetCamImage : ovGetCamImageBGR
CSHARP_EXPORT void ovPreStoreCamData(OVR::OvrvisionPro*g_ovOvrvision,int qt)
{
	if (g_ovOvrvision == NULL)
		return;

	g_ovOvrvision->PreStoreCamData((OVR::Camqt)qt);	//Renderer
}
CSHARP_EXPORT void ovPreStoreMemoryData(OVR::OvrvisionPro*g_ovOvrvision,int qt, void* data, bool remapData)
{
	if (g_ovOvrvision == NULL)
		return;

	g_ovOvrvision->PreStoreMemoryData((OVR::Camqt)qt, (const uchar*)data, remapData);	//Renderer
}

// int ovGetCamImage(unsigned char* pImage, int eye)
CSHARP_EXPORT void ovGetCamImageBGRA(OVR::OvrvisionPro*g_ovOvrvision, unsigned char* pImage, int eye)
{
	if(g_ovOvrvision==NULL)
		return;

	//Get image
	g_ovOvrvision->GetCamImageBGRA(pImage, (OVR::Cameye)eye);
}
CSHARP_EXPORT unsigned char* ovGetCamImageBGRAPointer(OVR::OvrvisionPro*g_ovOvrvision, int eye)
{
	if (g_ovOvrvision == NULL)
		return NULL;

	//Get image
	return g_ovOvrvision->GetCamImageBGRA((OVR::Cameye)eye);
}
// int ovGetCamImageRGB(unsigned char* pImage, int eye)
CSHARP_EXPORT void ovGetCamImageRGB(OVR::OvrvisionPro*g_ovOvrvision, unsigned char* pImage, int eye)
{
	if(g_ovOvrvision==NULL)
		return;

	//local var
	int i, srcj = 0;

	//Get image
	unsigned char* pData = g_ovOvrvision->GetCamImageBGRA((OVR::Cameye)eye);

	int length = g_ovOvrvision->GetCamWidth() * g_ovOvrvision->GetCamHeight() * BGR_DATASIZE;
	int offsetlen = g_ovOvrvision->GetCamPixelsize();

	//Image copy
	for (i = 0; i < length; i += 3) {
		//Left Eye
		pImage[i + 0] = pData[srcj + 2];	//R
		pImage[i + 1] = pData[srcj + 1];	//G
		pImage[i + 2] = pData[srcj + 0];	//B
		srcj += offsetlen;
	}
}
// int ovGetCamImageRGB(unsigned char* pImage, int eye)
CSHARP_EXPORT void ovGetCamImageBGR(OVR::OvrvisionPro*g_ovOvrvision, unsigned char* pImage, int eye)
{
	if (g_ovOvrvision == NULL)
		return;

	//local var
	int i, srcj = 0;

	//Get image
	unsigned char* pData = g_ovOvrvision->GetCamImageBGRA((OVR::Cameye)eye);

	int length = g_ovOvrvision->GetCamWidth() * g_ovOvrvision->GetCamHeight() * BGR_DATASIZE;
	int offsetlen = g_ovOvrvision->GetCamPixelsize();

	//Image copy
	for (i = 0; i < length; i += 3) {
		//Left Eye
		pImage[i + 0] = pData[srcj + 0];	//B
		pImage[i + 1] = pData[srcj + 1];	//G
		pImage[i + 2] = pData[srcj + 2];	//R
		srcj += offsetlen;
	}
}

// void ovGetCamImageForUnity(unsigned char* pImagePtr_Left, unsigned char* pImagePtr_Right, int qt, int useTrack)
CSHARP_EXPORT void ovGetCamImageForUnity(OVR::OvrvisionPro*g_ovOvrvision, unsigned char* pImagePtr_Left, unsigned char* pImagePtr_Right)
{
	if(g_ovOvrvision==NULL)
		return;

	//local var
	int i;

	//Get image
	unsigned char* pLeft = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_LEFT);
	unsigned char* pRight = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_RIGHT);

	int length = g_ovOvrvision->GetCamWidth() * g_ovOvrvision->GetCamHeight() * g_ovOvrvision->GetCamPixelsize();
	int offsetlen = g_ovOvrvision->GetCamPixelsize();

	//Image copy
	for (i = 0; i < length; i+=offsetlen) {
		//Left Eye
		pImagePtr_Left[i + 0] = pLeft[i + 2];	//R
		pImagePtr_Left[i + 1] = pLeft[i + 1];	//G
		pImagePtr_Left[i + 2] = pLeft[i + 0];	//B
		pImagePtr_Left[i + 3] = pLeft[i + 3];	//A

		//Right Eye
		pImagePtr_Right[i + 0] = pRight[i + 2];
		pImagePtr_Right[i + 1] = pRight[i + 1];
		pImagePtr_Right[i + 2] = pRight[i + 0];
		pImagePtr_Right[i + 3] = pRight[i + 3];
	}
}

//for Unity extern
extern float g_Time;
#if SUPPORT_D3D11
extern ID3D11Device* g_D3D11Device;
#endif
#if SUPPORT_D3D9
extern IDirect3DDevice9* g_D3D9Device;
#endif

// void ovGetCamImageForUnityNative(void* pTexPtr_Left, void* pTexPtr_Right, int qt, int useAR)
CSHARP_EXPORT void ovGetCamImageForUnityNative(OVR::OvrvisionPro*g_ovOvrvision, void* pTexPtr_Left, void* pTexPtr_Right)
{
	if (g_ovOvrvision == NULL || !g_ovOvrvision->isOpen())
		return;

	//Get image
	unsigned char* pLeft = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_LEFT);
	unsigned char* pRight = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_RIGHT);

	int length = g_ovOvrvision->GetCamWidth() * g_ovOvrvision->GetCamHeight() * g_ovOvrvision->GetCamPixelsize();
	int offsetlen = g_ovOvrvision->GetCamPixelsize();

	RenderAPI* renderer = GetRenderer();
	if (pTexPtr_Left)
	{
		renderer->BeginModifyTexture(pTexPtr_Left, 0, 0, 0);
		renderer->EndModifyTexture(pTexPtr_Left, g_ovOvrvision->GetCamWidth(), g_ovOvrvision->GetCamHeight(), g_ovOvrvision->GetCamPixelsize(), g_ovOvrvision->GetCamWidth() * offsetlen, pLeft);
	}

	if (pTexPtr_Right)
	{
		renderer->BeginModifyTexture(pTexPtr_Right, 0, 0, 0);
		renderer->EndModifyTexture(pTexPtr_Right, g_ovOvrvision->GetCamWidth(), g_ovOvrvision->GetCamHeight(), g_ovOvrvision->GetCamPixelsize(), g_ovOvrvision->GetCamWidth() * offsetlen, pRight);
	}
}
//for GL.IssuePluginEvent
static void __stdcall ovGetCamImageForUnityNativeEvent(int eventID)
{
	GLRenderCall call;
	{
		ScopedLock l(&g_mutex);
		if (g_calls.size() == 0)
			return;
		call = g_calls.front();
		g_calls.pop_front();
	}
	ovGetCamImageForUnityNative(call.ovOvrvision, call.callTexture2DLeft, call.callTexture2DRight);
}
CSHARP_EXPORT UnityRenderNative __stdcall ovGetCamImageForUnityNativeGLCall(OVR::OvrvisionPro*g_ovOvrvision, void* pTexPtr_Left, void* pTexPtr_Right)
{
	GLRenderCall call;
	
	call.callTexture2DLeft = pTexPtr_Left;
	call.callTexture2DRight = pTexPtr_Right;
	call.ovOvrvision = g_ovOvrvision;
	{
		ScopedLock l(&g_mutex);
		g_calls.push_back(call);
	}
	return ovGetCamImageForUnityNativeEvent;
}

//This method will be detected if a hand is put in front of a camera. 
CSHARP_EXPORT int ovPutHandInFrontOfCamera(unsigned char thres_less, unsigned char* pImageBuf)
{
	return 0;// (int)g_ovOvrvision->PutHandInFrontOfCamera(thres_less, pImageBuf);
}

//Get image width
CSHARP_EXPORT int ovGetImageWidth(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCamWidth();
}

//Get image height
CSHARP_EXPORT int ovGetImageHeight(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCamHeight();
}

//Get image framerate
CSHARP_EXPORT int ovGetImageRate(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCamFramerate();
}

//Get buffer size
CSHARP_EXPORT int ovGetBufferSize(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCamBuffersize();
}

//Get buffer size
CSHARP_EXPORT int ovGetPixelSize(OVR::OvrvisionPro*g_ovOvrvision)
{
	if (g_ovOvrvision == NULL)
		return 0;

	return g_ovOvrvision->GetCamPixelsize();
}

//Set exposure
CSHARP_EXPORT void ovSetExposure(OVR::OvrvisionPro*g_ovOvrvision,int value)
{
	if(g_ovOvrvision==NULL)
		return;

	g_ovOvrvision->SetCameraExposure(value);
}
//Set exposure per sec
CSHARP_EXPORT int ovSetExposurePerSec(OVR::OvrvisionPro*g_ovOvrvision, float fps)
{
	if (g_ovOvrvision == NULL)
		return 0;

	return g_ovOvrvision->SetCameraExposurePerSec(fps);
}


//Set gain
CSHARP_EXPORT void ovSetGain(OVR::OvrvisionPro*g_ovOvrvision, int value)
{
	if(g_ovOvrvision==NULL)
		return;

	g_ovOvrvision->SetCameraGain(value);
}

//Set WhiteBalanceR ( manual only )
CSHARP_EXPORT void ovSetWhiteBalanceR(OVR::OvrvisionPro*g_ovOvrvision, int value)
{
	if(g_ovOvrvision==NULL)
		return;

	g_ovOvrvision->SetCameraWhiteBalanceR(value);
}

//Set WhiteBalanceG ( manual only )
CSHARP_EXPORT void ovSetWhiteBalanceG(OVR::OvrvisionPro*g_ovOvrvision, int value)
{
	if(g_ovOvrvision==NULL)
		return;

	g_ovOvrvision->SetCameraWhiteBalanceG(value);
}

//Set WhiteBalanceB ( manual only )
CSHARP_EXPORT void ovSetWhiteBalanceB(OVR::OvrvisionPro*g_ovOvrvision, int value)
{
	if(g_ovOvrvision==NULL)
		return;

	g_ovOvrvision->SetCameraWhiteBalanceB(value);
}

//Set WhiteBalance Auto
CSHARP_EXPORT void ovSetWhiteBalanceAuto(OVR::OvrvisionPro*g_ovOvrvision, int value)
{
	if (g_ovOvrvision == NULL)
		return;

	g_ovOvrvision->SetCameraWhiteBalanceAuto((bool)value);
}

//Set Backlight Compensation
CSHARP_EXPORT void ovSetBLC(OVR::OvrvisionPro*g_ovOvrvision, int value)
{
	if (g_ovOvrvision == NULL)
		return;

	g_ovOvrvision->SetCameraBLC(value);
}

//Set Camera SyncMode
CSHARP_EXPORT void ovSetCamSyncMode(OVR::OvrvisionPro*g_ovOvrvision, int value)
{
	if (g_ovOvrvision == NULL)
		return;

	g_ovOvrvision->SetCameraSyncMode((bool)value);
}

//Get exposure
CSHARP_EXPORT int ovGetExposure(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCameraExposure();
}

//Get gain
CSHARP_EXPORT int ovGetGain(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCameraGain();
}

//Get whiteBalanceR
CSHARP_EXPORT int ovGetWhiteBalanceR(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCameraWhiteBalanceR();
}

//Get whiteBalanceG
CSHARP_EXPORT int ovGetWhiteBalanceG(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCameraWhiteBalanceG();
}

//Get whiteBalanceB
CSHARP_EXPORT int ovGetWhiteBalanceB(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCameraWhiteBalanceB();
}

CSHARP_EXPORT int ovGetWhiteBalanceAuto(OVR::OvrvisionPro*g_ovOvrvision)
{
	if (g_ovOvrvision == NULL)
		return 0;

	return (int)g_ovOvrvision->GetCameraWhiteBalanceAuto();
}

//Get Backlight Compensation
CSHARP_EXPORT int ovGetBLC(OVR::OvrvisionPro*g_ovOvrvision)
{
	if (g_ovOvrvision == NULL)
		return 0;

	return g_ovOvrvision->GetCameraBLC();
}

//Get focalPoint
CSHARP_EXPORT float ovGetFocalPoint(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetCamFocalPoint();
}

//Get HMD Right-eye Gap
CSHARP_EXPORT float ovGetHMDRightGap(OVR::OvrvisionPro*g_ovOvrvision,int at)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->GetHMDRightGap(at);
}


//Save parameter
CSHARP_EXPORT int ovSaveCamStatusToEEPROM(OVR::OvrvisionPro*g_ovOvrvision)
{
	if(g_ovOvrvision==NULL)
		return 0;

	return g_ovOvrvision->CameraParamSaveEEPROM();
}
CSHARP_EXPORT void ovLoadCameraConfiguration(OVR::OvrvisionPro*g_ovOvrvision, const char* str)
{
	if (g_ovOvrvision == NULL)
		return;
	g_ovOvrvision->SetOVRSettings(str);
}

// void ovARRender(void)
CSHARP_EXPORT OVR::OvrvisionAR* ovARCreate(OVR::OvrvisionPro*g_ovOvrvision, float arMeter)
{
	return new OVR::OvrvisionAR(arMeter, g_ovOvrvision->GetCamWidth(),
		g_ovOvrvision->GetCamHeight(),
		g_ovOvrvision->GetCamFocalPoint());
}

CSHARP_EXPORT void ovARDestroy(OVR::OvrvisionAR* ar)
{
	delete ar;
}

CSHARP_EXPORT void ovARRender(OVR::OvrvisionAR* g_ovOvrvisionAR,OVR::OvrvisionPro*g_ovOvrvision )
{
	if (g_ovOvrvisionAR == NULL)
		return;

	unsigned char* pLeft = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_LEFT);
	g_ovOvrvisionAR->SetImageBGRA(pLeft);

	//Rendering
	g_ovOvrvisionAR->Render();
}

// int ovARGetData(float* mdata, int datasize) : mdata*FLOATDATA_DATA_OFFSET(10)
CSHARP_EXPORT bool ovARGetMarkerData(OVR::OvrvisionAR* g_ovOvrvisionAR, int id,float* mdata)
{

	int i;
	if (g_ovOvrvisionAR == NULL)
		return false;
	if (mdata == NULL)	
		return false;


	int marklen = g_ovOvrvisionAR->GetMarkerDataSize();
	OVR::OvMarkerData* dt = g_ovOvrvisionAR->GetMarkerData();

	for (i = 0; i < marklen; i++)
	{
		if (dt[i].id != id)
			continue;
		mdata[0] = dt[i].translate.x;
		mdata[1] = dt[i].translate.y;
		mdata[2] = dt[i].translate.z;
		mdata[3] = dt[i].quaternion.x;
		mdata[4] = dt[i].quaternion.y;
		mdata[5] = dt[i].quaternion.z;
		mdata[6] = dt[i].quaternion.w;
		mdata[7] = dt[i].centerPtOfImage.x;
		mdata[8] = dt[i].centerPtOfImage.y;
		return true;
	}
	return false;

}
CSHARP_EXPORT int ovARGetData( OVR::OvrvisionAR* g_ovOvrvisionAR,float* mdata, int datasize)
{
	int i;
	if (g_ovOvrvisionAR == NULL)
		return (-1);

	if (mdata == NULL)
		return (-1);

	int marklen = g_ovOvrvisionAR->GetMarkerDataSize();
	OVR::OvMarkerData* dt = g_ovOvrvisionAR->GetMarkerData();

	for (i = 0; i < marklen; i++)
	{
		int ioffset = i * FLOATDATA_DATA_OFFSET;
		if (i >= (datasize / FLOATDATA_DATA_OFFSET))
			break;

		mdata[ioffset + 0] = (float)dt[i].id;
		mdata[ioffset + 1] = dt[i].translate.x;
		mdata[ioffset + 2] = dt[i].translate.y;
		mdata[ioffset + 3] = dt[i].translate.z;
		mdata[ioffset + 4] = dt[i].quaternion.x;
		mdata[ioffset + 5] = dt[i].quaternion.y;
		mdata[ioffset + 6] = dt[i].quaternion.z;
		mdata[ioffset + 7] = dt[i].quaternion.w;
		mdata[ioffset + 8] = dt[i].centerPtOfImage.x;
		mdata[ioffset + 9] = dt[i].centerPtOfImage.y;
	}

	return marklen;	//S_OK
}

// void ovARSetMarkerSize(float value)
CSHARP_EXPORT void ovARSetMarkerSize(OVR::OvrvisionAR* g_ovOvrvisionAR,float value)
{
	if (g_ovOvrvisionAR == NULL)
		return;

	g_ovOvrvisionAR->SetMarkerSizeMeter(value);
}

// float ovARGetMarkerSize()
CSHARP_EXPORT float ovARGetMarkerSize(OVR::OvrvisionAR* g_ovOvrvisionAR)
{
	if (g_ovOvrvisionAR == NULL)
		return 0;

	return g_ovOvrvisionAR->GetMarkerSizeMeter();
}

// void ov3DInstantTraking_Metaio(int value)
CSHARP_EXPORT void ov3DInstantTraking_Metaio(OVR::OvrvisionAR* g_ovOvrvisionAR,int value)
{
	if (g_ovOvrvisionAR == NULL)
		return;

	g_ovOvrvisionAR->SetInstantTraking((bool)value);
}

////////////// Ovrvision Tracking //////////////

CSHARP_EXPORT OVR::OvrvisionTracking* ovTrackCreate(OVR::OvrvisionPro*g_ovOvrvision)
{
	return new OVR::OvrvisionTracking(g_ovOvrvision->GetCamWidth(),
		g_ovOvrvision->GetCamHeight(), g_ovOvrvision->GetCamFocalPoint());
}

CSHARP_EXPORT void ovTrackDestroy(OVR::OvrvisionTracking* ar)
{
	delete ar;
}
// ovTrackRender
CSHARP_EXPORT void ovTrackRender(OVR::OvrvisionTracking*g_ovOvrvisionTrack, OVR::OvrvisionPro*g_ovOvrvision,bool calib, bool point)
{
	if (g_ovOvrvisionTrack == NULL)
		return;

	unsigned char* pLeft = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_LEFT);
	unsigned char* pRight = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_RIGHT);
	g_ovOvrvisionTrack->SetImageBGRA(pLeft, pRight);

	g_ovOvrvisionTrack->Render(calib, point);
}

CSHARP_EXPORT int ovGetTrackData(OVR::OvrvisionTracking*g_ovOvrvisionTrack,float* mdata)
{
	if (g_ovOvrvisionTrack == NULL)
		return 0;

	if (mdata == NULL)
		return (-1);

	mdata[0] = g_ovOvrvisionTrack->FingerPosX();
	mdata[1] = g_ovOvrvisionTrack->FingerPosY();
	mdata[2] = g_ovOvrvisionTrack->FingerPosZ();

	if (mdata[2] <= 0.0f || mdata[2] >= 1.0f)	//z0.0~1.0
		return 0;

	return 1;
}

CSHARP_EXPORT void ovTrackingCalibReset(OVR::OvrvisionTracking*g_ovOvrvisionTrack)
{
	if (g_ovOvrvisionTrack == NULL)
		return;

	g_ovOvrvisionTrack->SetHue();
}

////////////// Ovrvision AR //////////////
/*
// void ovARRender(void)
CSHARP_EXPORT void ovARRender()
{
	if(g_ovOvrvisionAR==NULL)
		return;

	unsigned char* pLeft = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_LEFT);
	g_ovOvrvisionAR->SetImageBGRA(pLeft);

	//Rendering
	g_ovOvrvisionAR->Render();
}

// int ovARGetData(float* mdata, int datasize) : mdata*FLOATDATA_DATA_OFFSET(10)
CSHARP_EXPORT int ovARGetData(float* mdata, int datasize)
{
	int i;
	if(g_ovOvrvisionAR==NULL)
		return (-1);

	if (mdata == NULL)
		return (-1);

	int marklen = g_ovOvrvisionAR->GetMarkerDataSize();
	OVR::OvMarkerData* dt = g_ovOvrvisionAR->GetMarkerData();

	for(i = 0; i < marklen; i++)
	{
		int ioffset = i * FLOATDATA_DATA_OFFSET;
		if(i >= (datasize / FLOATDATA_DATA_OFFSET))
			break;

		mdata[ioffset+0] = (float)dt[i].id;
		mdata[ioffset+1] = dt[i].translate.x;
		mdata[ioffset+2] = dt[i].translate.y;
		mdata[ioffset+3] = dt[i].translate.z;
		mdata[ioffset+4] = dt[i].quaternion.x;
		mdata[ioffset+5] = dt[i].quaternion.y;
		mdata[ioffset+6] = dt[i].quaternion.z;
		mdata[ioffset+7] = dt[i].quaternion.w;
		mdata[ioffset+8] = dt[i].centerPtOfImage.x;
		mdata[ioffset+9] = dt[i].centerPtOfImage.y;
	}

	return marklen;	//S_OK
}

// void ovARSetMarkerSize(float value)
CSHARP_EXPORT void ovARSetMarkerSize(float value)
{
	if(g_ovOvrvisionAR==NULL)
		return;

	g_ovOvrvisionAR->SetMarkerSizeMeter(value);
}

// float ovARGetMarkerSize()
CSHARP_EXPORT float ovARGetMarkerSize()
{
	if(g_ovOvrvisionAR==NULL)
		return 0;

	return g_ovOvrvisionAR->GetMarkerSizeMeter();
}

// void ov3DInstantTraking_Metaio(int value)
CSHARP_EXPORT void ov3DInstantTraking_Metaio(int value)
{
	if(g_ovOvrvisionAR==NULL)
		return;

	g_ovOvrvisionAR->SetInstantTraking((bool)value);
}

////////////// Ovrvision Tracking //////////////

// ovTrackRender
CSHARP_EXPORT void ovTrackRender(bool calib, bool point)
{
	if (g_ovOvrvisionTrack == NULL)
		return;

	unsigned char* pLeft = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_LEFT);
	unsigned char* pRight = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_RIGHT);
	g_ovOvrvisionTrack->SetImageBGRA(pLeft, pRight);

	g_ovOvrvisionTrack->Render(calib, point);
}

CSHARP_EXPORT int ovGetTrackData(float* mdata)
{
	if (g_ovOvrvisionTrack == NULL)
		return 0 ;

	if (mdata == NULL)
		return (-1);

	mdata[0] = g_ovOvrvisionTrack->FingerPosX();
	mdata[1] = g_ovOvrvisionTrack->FingerPosY();
	mdata[2] = g_ovOvrvisionTrack->FingerPosZ();

	if (mdata[2] <= 0.0f || mdata[2] >= 1.0f)	//z0.0~1.0
		return 0;

	return 1;
}

CSHARP_EXPORT void ovTrackingCalibReset()
{
	if (g_ovOvrvisionTrack == NULL)
		return;

	g_ovOvrvisionTrack->SetHue();
}

////////////// Ovrvision Calibration //////////////

CSHARP_EXPORT void ovCalibInitialize(int pattern_size_w, int pattern_size_h, double chessSizeMM)
{
	if(g_ovOvrvision == NULL)
		return;

	if(g_ovOvrvisionCalib)
		delete g_ovOvrvisionCalib;

	g_ovOvrvisionCalib = new OVR::OvrvisionCalibration(
		g_ovOvrvision->GetCamWidth(),g_ovOvrvision->GetCamHeight(),
		pattern_size_w,pattern_size_h,chessSizeMM);
}

CSHARP_EXPORT void ovCalibClose()
{
	if (g_ovOvrvisionCalib)
		delete g_ovOvrvisionCalib;
}

CSHARP_EXPORT int ovCalibFindChess()
{
	if(g_ovOvrvisionCalib == NULL)
		return 0;

	g_ovOvrvision->PreStoreCamData(OVR::OV_CAMQT_DMS);	//ReRenderer
	unsigned char* pLeft = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_LEFT);
	unsigned char* pRight = g_ovOvrvision->GetCamImageBGRA(OVR::OV_CAMEYE_RIGHT);

	return g_ovOvrvisionCalib->FindChessBoardCorners(pLeft, pRight);
}

CSHARP_EXPORT void ovCalibSolveStereoParameter()
{
	if (g_ovOvrvision == NULL)
		return;
	if(g_ovOvrvisionCalib == NULL)
		return;

	g_ovOvrvisionCalib->SolveStereoParameter();
	g_ovOvrvisionCalib->SaveCalibrationParameter(g_ovOvrvision);	//default 
	//g_ovOvrvisionCalib->SaveCalibrationParameterToEEPROM();
}

CSHARP_EXPORT int ovCalibGetImageCount()
{
	if(g_ovOvrvisionCalib == NULL)
		return -1;

	return g_ovOvrvisionCalib->GetImageCount();
}

*/

#ifdef __cplusplus
}
#endif
