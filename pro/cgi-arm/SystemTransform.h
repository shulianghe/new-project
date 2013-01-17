/** @file    SystemTransform.h
  * @note    HANGZHOU Hikvison Software Co.,Ltd.All Right Reserved.
  * @brief   SystemTransform header file
  * 
  * @author  PlaySDK
  * @date    27/8/2012
  * 
  * @note
  *
  * @warning  Windows 32bit /Linux32 bit version
  */

#ifndef _SYSTEM_TRANSFORM_H_
#define _SYSTEM_TRANSFORM_H_

#ifdef WIN32
	#if defined(_WINDLL)
	    #define SYSTRANS_API  __declspec(dllexport) 
	#else 
	    #define SYSTRANS_API  __declspec(dllimport) 
    #endif
#else
    #ifndef __stdcall
	    #define __stdcall
    #endif

	#ifndef SYSTRANS_API
	    #define  SYSTRANS_API
	#endif
#endif

#define SWITCH_BY_TIME			2

#define SUBNAME_BY_GLOBALTIME	2

/************************************************************************
* 状态码定义
************************************************************************/
#define SYSTRANS_OK				0x00000000
#define SYSTRANS_E_HANDLE		0x80000000	//转换句柄错误
#define SYSTRANS_E_SUPPORT		0x80000001	//类型不支持
#define SYSTRANS_E_RESOURCE		0x80000002	//资源申请或释放错误
#define SYSTRANS_E_PARA			0x80000003	//参数错误
#define SYSTRANS_E_PRECONDITION 0x80000004  //前置条件未满足，调用顺序
#define SYSTRANS_E_OVERFLOW     0x80000005  //缓存溢出
#define SYSTRANS_E_STOP			0x80000006  //停止状态
#define SYSTRANS_E_FILE         0x80000007  //文件错误

/************************************************************************
* 目标封装格式类型
************************************************************************/
typedef enum SYSTEM_TYPE
{
	TRANS_SYSTEM_HIK		= 0x1,	//海康文件层，可用于传输和存储
	TRANS_SYSTEM_MPEG2_PS	= 0x2,	//PS文件层，主要用于存储，也可用于传输
	TRANS_SYSTEM_MPEG2_TS	= 0x3,	//TS文件层，主要用于传输，也可用于存储
	TRANS_SYSTEM_RTP		= 0x4,	//RTP文件层，用于传输
	TRANS_SYSTEM_MPEG4		= 0x5,	//MPEG4文件层，用于存储
};

typedef struct SYS_TRANS_PARA 
{
	unsigned char* pSrcInfo;		//海康设备出的媒体信息头
	unsigned long  dwSrcInfoLen;	//当前固定为40
	SYSTEM_TYPE    enTgtType;
	unsigned long  dwTgtPackSize;	//如果目标为RTP，PS/TS等封装格式时，设定每个包大小的上限
} SYS_TRANS_PARA;

typedef struct AUTO_SWITCH_PARA
{
	unsigned long dwSwitchFlag;		    //SWITCH_BY_TIME：通过时间来切换
	unsigned long dwSwitchValue;	    //时间以分钟为单位
	unsigned long dwSubNameFlag;	    //SUBNAME_BY_GLOBALTIME: 文件名以全局时间区分
	char          szMajorName[128];     //如szMajorName = c:\test,切换文件后的名称 = c:\test_年月日时分秒.mp4
} AUTO_SWITCH_PARA;

#define TRANS_SYSHEAD				1	//系统头数据
#define TRANS_STREAMDATA			2	//视频流数据（包括复合流和音视频分开的视频流数据）
#define TRANS_AUDIOSTREAMDATA		3	//音频流数据
#define TRANS_PRIVTSTREAMDATA       4   //Private stream
#define TRANS_PACKPARAM             5   //Parameter for raw data

typedef struct OUTPUTDATA_INFO 
{
	unsigned char* pData;          //回调数据缓存，该指针请勿用于异步的传递
	unsigned long dwDataLen;       //回掉数据长度
	unsigned long dwDataType;      //数据类型，如TRANS_SYSHEAD,TRANS_STREAMDATA
} OUTPUTDATA_INFO;


typedef enum DATA_TYPE 
{
	MULTI_DATA,
 	VIDEO_DATA,
 	AUDIO_DATA,
 	PRIVATE_DATA,
    VIDEO_PARA,
	AUDIO_PARA,
	PRIVATE_PARA
};

//下面的结构体针对上面的VIDEO_PARA数据类型
typedef struct _HK_SYSTEM_TIME_
{
    unsigned int		   dwYear;
    unsigned int           dwMonth;
    unsigned int		   dwDay;
    unsigned int		   dwHour;
    unsigned int		   dwMinute;
    unsigned int		   dwSecond;
    unsigned int           dwMilliSecond;
    unsigned int           dwReserved;
} HK_SYSTEM_TIME;

typedef struct _HK_VIDEO_PACK_PARA_ 
{
    unsigned int   dwFrameNum;
    unsigned int   dwTimeStamp;
    float          fFrameRate;
    unsigned int   dwReserved;
    HK_SYSTEM_TIME stSysTime;
    
} HK_VIDEO_PACK_PARA;

//下面的结构体针对上面的AUDIO_PARA数据类型
typedef struct _HK_AUDIO_PACK_PARA_ 
{
    unsigned int          dwChannels;
    unsigned int		  dwBitsPerSample;
    unsigned int          dwSampleRate;
    unsigned int          dwBitRate;
	unsigned int          dwTimeStamp;
	unsigned int          dwReserved[3];
} HK_AUDIO_PACK_PARA;

//下面的结构体针对上面的PRIVATE_PARA数据类型
typedef struct _HK_PRIVATE_PACK_PARA_ 
{
    unsigned int          dwPrivateType;
    unsigned int		  dwDataType;
    unsigned int          dwSycVideoFrame;
    unsigned int          dwReserved;
	unsigned int          dwTimeStamp;
    unsigned int          dwReserved1[2];
} HK_PRIVATE_PACK_PARA;

#ifdef __cplusplus
	extern "C" {
#endif
/************************************************************************
* 函数名：SYSTRANS_Create                                                  
* 功能：  通过源和目标的封装类型来创建封装格式转换句柄
* 参数：  phTrans	   - 返回的句柄
*		  pstTransInfo - 转换信息数据指针
* 返回值：状态码
************************************************************************/
SYSTRANS_API long __stdcall SYSTRANS_Create(void** phTrans, SYS_TRANS_PARA* pstTransInfo);

/************************************************************************
* 函数名：SYSTRANS_Start                                                 
* 功能：  开始封装格式转换
* 参数：  hTrans	- 转换句柄
*		  szSrcPath - 源文件路径，如果置NULL，表明为流
*		  szTgtPath - 目标文件路径，如果置NULL，表明为流
* 返回值：状态码
************************************************************************/
SYSTRANS_API long __stdcall SYSTRANS_Start(void* hTrans, const char* szSrcPath, const char* szTgtPath);

/************************************************************************
* 函数名：SYSTRANS_AutoSwitch                                                 
* 功能：  目标为文件时，自动切换存储文件
* 参数：  hTrans	- 转换句柄
*		  pstPara   - 自动切换文件的参数结构指针
* 返回值：状态码 
************************************************************************/
SYSTRANS_API long __stdcall SYSTRANS_AutoSwitch(void* hTrans, AUTO_SWITCH_PARA* pstPara);

/************************************************************************
* 函数名：SYSTRANS_ManualSwitch                                                 
* 功能：  目标为文件时，手动切换存储文件
* 参数：  hTrans	- 转换句柄
*		  szTgtPath - 下一存储文件的路径
* 返回值：状态码 
************************************************************************/
SYSTRANS_API long __stdcall SYSTRANS_ManualSwitch(void* hTrans, const char* szTgtPath);

/************************************************************************
* 函数名：SYSTRANS_InputData                                                 
* 功能：  源为流模式，塞入数据
* 参数：  hTrans	- 转换句柄
*		  pData		- 源，流数据指针
*		  dwDataLen - 流数据大小
*		  enType	- 码流类型，暂未使用，统一用MULTI_DATA
* 返回值：状态码
************************************************************************/
SYSTRANS_API long __stdcall SYSTRANS_InputData(void* hTrans, DATA_TYPE enType, unsigned char* pData, unsigned long dwDataLen);

/************************************************************************
* 函数名：SYSTRANS_GetTransPercent                                                 
* 功能：  转文件模式时，获得转换百分比，暂时只支持源是HIK，PS和MPEG4
* 参数：  hTrans	 - 转换句柄
*		  pdwPercent - 转换百分比
* 返回值：状态码
************************************************************************/

SYSTRANS_API long __stdcall SYSTRANS_GetTransPercent(void* hTrans,unsigned int* pdwPercent);

/************************************************************************
* 函数名：SYSTRANS_RegisterOutputDataCallBack                                                 
* 功能：  目标为流模式，注册转换后数据回调
* 参数：  hTrans				- 转换句柄
*		  OutputDataCallBack	- 函数指针
*		  dwUser				- 用户数据
* 返回值：状态码
* 说明：  3GPP不支持回调
************************************************************************/
SYSTRANS_API long __stdcall SYSTRANS_RegisterOutputDataCallBack(void* hTrans, void (__stdcall * OutputDataCallBack)(OUTPUTDATA_INFO* pDataInfo, unsigned long dwUser), unsigned long dwUser);


/************************************************************************
* 函数名：SYSTRANS_RegisterOutputDataCallBackEx                                                 
* 功能：  目标为流模式，注册转换后数据回调
* 参数：  hTrans				- 转换句柄
*		  OutputDataCallBack	- 函数指针
*		  dwUser				- 用户数据
* 返回值：状态码
* 说明：  3GPP不支持回调
************************************************************************/
SYSTRANS_API long __stdcall SYSTRANS_RegisterOutputDataCallBackEx(void* hTrans, void (__stdcall * OutputDataCallBack)(OUTPUTDATA_INFO* pDataInfo, void* pUser), void* pUser);

/************************************************************************
* 函数名：SYSTRANS_Stop                                                 
* 功能：  停止转换
* 参数：  hTrans	 - 转换句柄
* 返回值：状态码
************************************************************************/
SYSTRANS_API long __stdcall SYSTRANS_Stop(void* hTrans);

/************************************************************************
* 函数名：SYSTRANS_Release                                                 
* 功能：  释放转换句柄
* 参数：  hTrans	 - 转换句柄
* 返回值：状态码
************************************************************************/
SYSTRANS_API long __stdcall SYSTRANS_Release(void* hTrans);


#ifdef __cplusplus
	}
#endif

#endif //_SYSTEM_TRANSFORM_H_
