#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include   <unistd.h> 
#endif

#include "HCNetSDK.h"
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#define HPR_OK 0
#define HPR_ERROR 1
char File[64] = "./save.mp4";
void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    char tempbuf[256] = {0};
    switch(dwType) 
    {
    case EXCEPTION_RECONNECT:    //预览时重连
        printf("----------reconnect--------%d\n", time(NULL));
    break;
	default:
    break;
    }
}

int main() {

	//---------------------------------------
	// 初始化
	NET_DVR_Init();
	//设置连接时间与重连时间
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);

	//---------------------------------------
	// 注册设备
	LONG lUserID;
	NET_DVR_DEVICEINFO_V30 struDeviceInfo;
	lUserID = NET_DVR_Login_V30("192.168.1.64", 8000, "admin", "12345", &struDeviceInfo);
	if (lUserID < 0)
	{
		printf("Login error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return 1;
	}

	//---------------------------------------
	//设置异常消息回调函数
	NET_DVR_SetExceptionCallBack_V30(0, NULL,g_ExceptionCallBack, NULL);

	//---------------------------------------
	//启动预览并设置回调数据流
	LONG lRealPlayHandle;
	NET_DVR_CLIENTINFO ClientInfo = {0};
	ClientInfo.hPlayWnd = 0;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
	ClientInfo.lChannel     = 1;       //预览通道号
	ClientInfo.lLinkMode    = 0;       //最高位(31)为0表示主码流，为1表示子码流0～30位表示连接方式：0－TCP方式；1－UDP方式；2－多播方式；3－RTP方式;
	ClientInfo.sMultiCastIP = NULL;   //多播地址，需要多播预览时配置
  
	BOOL bPreviewBlock = false;       //请求码流过程是否阻塞，0：否，1：是
	lRealPlayHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo, NULL, NULL, 0);
	if (lRealPlayHandle < 0)
	{
		printf("NET_DVR_RealPlay_V30 error:%d\n",NET_DVR_GetLastError());
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup();
		return 1;
	}
  
	printf("Start save...\n");
	int iRet,fd;	
	fd = open(File,O_RDWR|O_CREAT|O_TRUNC|O_APPEND,S_IRWXU|S_IRGRP|S_IROTH);//先调用open创建之后，不许要root权限即可录制，避免文加大小为0
	close(fd);
	iRet = NET_DVR_SaveRealData(lRealPlayHandle,File);
    if(!iRet)
    {
		printf("pyd1---NET_DVR_SaveRealData error:%s\n",NET_DVR_GetLastError());
        NET_DVR_StopSaveRealData(lRealPlayHandle);
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();  
        return 1;
	}
	printf("saving...\n");
	sleep(10);
	
	printf("save data success!\n");
	
	//---------------------------------------
	//关闭预览
	NET_DVR_StopSaveRealData(lRealPlayHandle);
	NET_DVR_StopRealPlay(lRealPlayHandle);
	//注销用户
	NET_DVR_Logout(lUserID);
	//释放SDK资源
	NET_DVR_Cleanup();

	return 0;
}
