#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "unistd.h"
#include "HCNetSDK.h"
#include "LinuxPlayM4.h"
#include <time.h>
using namespace std;

#define DEBUG 1

LONG lPort; //全局的播放库port号

void CALLBACK g_RealDataCallBack_V30(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,DWORD dwBufSize,void* dwUser)
{
   // HWND hWnd=GetConsoleWindow();
#ifdef DEBUG
	printf("realplay function..");
#endif
	HWND hWnd = NET_DVR_GetRealPlayerIndex(lRealHandle);
	switch (dwDataType)
	{
		case NET_DVR_SYSHEAD: //系统头

			if (!PlayM4_GetPort(&lPort))  //获取播放库未使用的通道号
			{
				break;
			}
			//m_iPort = lPort; //第一次回调的是系统头，将获取的播放库port号赋值给全局port，下次回调数据时即使用此port号播放
			if (dwBufSize > 0)
			{
				if (!PlayM4_SetStreamOpenMode(lPort, STREAME_REALTIME))  //设置实时流播放模式
				{
					break;
				}

				if (!PlayM4_OpenStream(lPort, pBuffer, dwBufSize, 1024*1024)) //打开流接口
				{
					break;
				}

				if (!PlayM4_Play(lPort, hWnd)) //播放开始
				{
					printf("播放开始...\n");
					break;
				}
			}
		case NET_DVR_STREAMDATA:   //码流数据
			if (dwBufSize > 0 && lPort != -1)
			{
				if (!PlayM4_InputData(lPort, pBuffer, dwBufSize))
				{
					break;
				}	 
			}
		default:
			break;
	}
}

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
#ifdef DEBUG
	printf("callback...");
#endif
    char tempbuf[256] = {0};
    switch(dwType) 
    {
		case EXCEPTION_AUDIOEXCHANGE:		//语音对讲时网络异常
			sprintf(tempbuf,"语音对讲时网络异常!!!");
			printf("%s",tempbuf);
			//TODO: 关闭语音对讲
			break;
		case EXCEPTION_ALARM:			//报警上传时网络异常
			sprintf(tempbuf,"报警上传时网络异常!!!");
			printf("%s",tempbuf);
			//TODO: 关闭报警上传
			break;
		case EXCEPTION_PREVIEW:			//网络预览时异常
			sprintf(tempbuf,"网络预览时网络异常!!!");
			printf("%s",tempbuf);	
			//TODO: 关闭网络预览
			break;
		case EXCEPTION_SERIAL:			//透明通道传输时异常
			sprintf(tempbuf,"透明通道传输时网络异常!!!");
			printf("%s",tempbuf);
			//TODO: 关闭透明通道
			break;
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
       exit(-1);
  }
	printf("longin success!!\n");
  //---------------------------------------
  //设置异常消息回调函数
  if(! NET_DVR_SetExceptionCallBack_V30(0, NULL,g_ExceptionCallBack, NULL))
  {
	 printf("set exceptioncallback error.%d\n",NET_DVR_GetLastError());
	 NET_DVR_Cleanup();
	 exit(-1);
  }
#ifdef DEBUG
 	printf("set exception success!\n");
#endif
  //---------------------------------------
  //启动预览并设置回调数据流
  LONG lRealPlayHandle;
  NET_DVR_CLIENTINFO ClientInfo = {0};
  ClientInfo.hPlayWnd = NULL;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
  ClientInfo.lChannel     = 1;       //预览通道号
  ClientInfo.lLinkMode    = 0;       //最高位(31)为0表示主码流，为1表示子码流0～30位表示连接方式：0－TCP方式；1－UDP方式；2－多播方式；3－RTP方式;
  ClientInfo.sMultiCastIP = NULL;   //多播地址，需要多播预览时配置
  
  BOOL bPreviewBlock = false;       //请求码流过程是否阻塞，0：否，1：是
#ifdef DEBUG
  printf("enter...\n");
#endif
  lRealPlayHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo, g_RealDataCallBack_V30, NULL, 0);
  if (lRealPlayHandle < 0)
  {
      printf("NET_DVR_RealPlay_V30 error\n");
	  NET_DVR_Logout(lUserID);
      NET_DVR_Cleanup();
      return -1;
  }
 printf("lRealPlayHandle=%d.\n",lRealPlayHandle);

  //---------------------------------------
  //关闭预览
  NET_DVR_StopRealPlay(lRealPlayHandle);
  //注销用户
  NET_DVR_Logout_V30(lUserID);
  NET_DVR_Cleanup();
#ifdef DEBUG
  printf("login out!\n");
#endif

  return 0;
}
