
#ifndef __HCAMERA_H__
#define __HCAMERA_H__

#define HCAMERA_API extern "C" __declspec(dllexport)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 常量、结构体定义

#define PINTYPE_CAPTURE		0
#define PINTYPE_STILL		1

#define VIDEOFORMAT_MJPG	0
#define VIDEOFORMAT_YUY2	1
#define VIDEOFORMAT_RGB24	2
#define VIDEOFORMAT_RGB32	3

typedef struct _HFormatResolutionInfo
{
	int videoFormat;	// 视频格式
	int videoW;			// 视频宽度
	int videoH;			// 视频高度
} HFormatResolutionInfo;


#define POWERLINE_50HZ	1		
#define POWERLINE_60HZ	2

#define PROPERTY_DISABLE				0x00
#define PROPERTY_ENABLE_AUTO			0x01		
#define PROPERTY_ENABLE_MANUAL			0x02
#define PROPERTY_ENABLE_AUTOANDMANUAL	0x03

typedef enum {
	PropertyFlag_Auto = 0x0001,
	PropertyFlag_Manual = 0x0002
} PropertyFlag;

typedef enum {
	Property_Brightness				= 0,
	Property_Contrast				= ( Property_Brightness + 1 ) ,
	Property_Hue					= ( Property_Contrast + 1 ) ,
	Property_Saturation				= ( Property_Hue + 1 ) ,
	Property_Sharpness				= ( Property_Saturation + 1 ) ,
	Property_Gamma					= ( Property_Sharpness + 1 ) ,
	Property_ColorEnable			= ( Property_Gamma + 1 ) ,
	Property_WhiteBalance			= ( Property_ColorEnable + 1 ) ,
	Property_BacklightCompensation	= ( Property_WhiteBalance + 1 ) ,
	Property_Gain					= ( Property_BacklightCompensation + 1 ) ,
	Property_PowerLine				= ( Property_Gain + 4 )
} PropertyProcAmp;

typedef enum {
	Property_Pan					= 0,
	Property_Tilt					= ( Property_Pan + 1 ) ,
	Property_Roll					= ( Property_Tilt + 1 ) ,
	Property_Zoom					= ( Property_Roll + 1 ) ,
	Property_Exposure				= ( Property_Zoom + 1 ) ,
	Property_Iris					= ( Property_Exposure + 1 ) ,
	Property_Focus					= ( Property_Iris + 1 ) ,
	Property_LowlightCompensation   = 19
} PropertyCameraControl;

typedef enum {
	SampleGrabberMode_NonCallback =0,
	SampleGrabberMode_BufferCB,
	SampleGrabberMode_SampleCB
} SampleGrabberMode;

typedef void (*SampleGrabberCallbackFunc)(unsigned char *pFrameBuf, long lFrameLength);

typedef struct _CameraParam{
	int capVideoW;							// for capture PIN
	int capVideoH;
	int capVideoFormat;
	int capGrabFormat;						
	SampleGrabberMode capMode;
	SampleGrabberCallbackFunc capCBFunc;
	int stiEnable;
	int stiVideoW;							// for still PIN
	int stiVideoH;
	int stiVideoFormat;
	int stiGrabFormat;
	SampleGrabberMode stiMode;
	SampleGrabberCallbackFunc stiCBFunc;
} CameraParam;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 函数定义


///
/// 宽窄字符转换
///
HCAMERA_API void hConvertWCharToChar(wchar_t* wStr, char* cStr);
HCAMERA_API void hConvertCharToWChar(char* cStr, wchar_t* wStr);

///
/// 枚举/释放视频设备(枚举时返回已连接的视频设备数量，-1表示错误，两个函数必须成对使用)
///
HCAMERA_API  int hEnumVideoDevice(void);
HCAMERA_API void hFreeVideoDevice(void);

///
/// 获取视频设备信息
///
HCAMERA_API BOOL hGetVideoDeviceFriendlyName(int devIndex, wchar_t* szFriendlyName, int Length);
HCAMERA_API BOOL hGetVideoDeviceVIDPID(int devIndex, int *vid, int *pid);


///
/// 获取视频设备所支持的格式和分辨率
///
HCAMERA_API BOOL hQuerySupportResolutionAndFormat(int devIndex, int pinType, HFormatResolutionInfo *pFRInfoList, int *FRInfoListLength);

///
/// 打开/关闭视频设备
///
HCAMERA_API BOOL hOpenCameraDevice(int devIndex, CameraParam param);
HCAMERA_API void hCloseCameraDevice(int devIndex);


HCAMERA_API BOOL hOpenCameraDevice2(int devIndex1, CameraParam param1, int devIndex2, CameraParam param2);
HCAMERA_API void hCloseCameraDevice2(int devIndex1, int devIndex2);

///
/// 设置视频窗口
///
HCAMERA_API BOOL hSetVideoWindow(int devIndex, HWND hwnd);
HCAMERA_API BOOL hSetVideoWindowPosition(int devIndex, int x, int y, int w, int h);

///
/// 设置视频帧率
///
HCAMERA_API BOOL hGetFrameRate(int devIndex, int pinType, int formatIndex, int videoW, int videoH, double *pList, int *len);
HCAMERA_API BOOL hSetFrameRate(int devIndex, double framerate);

HCAMERA_API BOOL hAddGraphToRot(int devIndex, DWORD *pdwRegister);
HCAMERA_API void hRemoveGraphFromRot(DWORD pdwRegister);

///
/// 开启/关闭视频流
///
HCAMERA_API void hPlay(int devIndex);
HCAMERA_API void hStop(int devIndex);

///
/// 获取视频帧
///
HCAMERA_API BOOL hGrabFrame(int devIndex, unsigned char *pbuf, long *length);

///
/// 显示视频属性页
///
HCAMERA_API BOOL hDisplayPropertyPages(int devIndex, HWND hwnd);

///
/// 获取/设置视频属性
///
HCAMERA_API BOOL hGetVideoProcAmpInfo(int devIndex, PropertyProcAmp prop, long *min, long *max, long *step, long *deft, long *enable);
HCAMERA_API BOOL hGetVideoProcAmpValue(int devIndex, PropertyProcAmp prop, long *value, PropertyFlag *flag);
HCAMERA_API BOOL hSetVideoProcAmpValue(int devIndex, PropertyProcAmp prop, long value, PropertyFlag flag);

HCAMERA_API BOOL hGetVideoCameraControlInfo(int devIndex, PropertyCameraControl prop, long *min, long *max, long *step, long *deft, long *enable);
HCAMERA_API BOOL hGetVideoCameraControlValue(int devIndex, PropertyCameraControl prop, long *value, PropertyFlag *flag);
HCAMERA_API BOOL hSetVideoCameraControlValue(int devIndex, PropertyCameraControl prop, long value, PropertyFlag flag);

///
/// DSP函数
///
HCAMERA_API BOOL hRclReadReg8(int devIndex, unsigned short regAddr, unsigned char *regVal);
HCAMERA_API BOOL hRclWriteReg8(int devIndex, unsigned short regAddr, unsigned char regVal);
HCAMERA_API BOOL hRclReadReg16(int devIndex, unsigned short regAddr, unsigned short *regVal);
HCAMERA_API BOOL hRclWriteReg16(int devIndex, unsigned short regAddr, unsigned short regVal);
HCAMERA_API BOOL hRclReadMem(int devIndex, unsigned short regAddr, unsigned char *regVal);
HCAMERA_API BOOL hRclWriteMem(int devIndex, unsigned short regAddr, unsigned char regVal);
HCAMERA_API BOOL hRclReadMemBuf(int devIndex, unsigned short regAddr, unsigned char *pregVal, int len);
HCAMERA_API BOOL hRclWriteMemBuf(int devIndex, unsigned short regAddr, unsigned char *pregVal, int len);

HCAMERA_API BOOL hRclReadFlash(int devIndex, unsigned int addr, unsigned char *pval, int len);
HCAMERA_API BOOL hRclWriteFlash(int devIndex, unsigned int addr, unsigned char *pval, int len);

#endif	// __HCAMERA_H__