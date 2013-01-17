#include <stdio.h>
#include "HCNetSDK.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

extern "C"
{
	#include "cgic.h"
}

#define HPR_OK 1
#define HPR_ERROR 0
#define LOCAL_IP "192.168.1.130"
#define LOCAL_PORT 7200

int Demo_AlarmFortify();
int saveData();
int saveFlag = 0;
char fileName[128] = "./test.mp4";
LONG lUserID;

void CALLBACK g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    char tempbuf[256] = {0};
    switch(dwType) 
    {
    case EXCEPTION_RECONNECT:    //Ô¤ÀÀÊ±ÖØÁ¬
        printf("----------reconnect--------%d\n", time(NULL));
        saveFlag = 1;
    break;
	default:
    break;
    }
}

void CALLBACK MessageCallback(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
	int i;
	NET_DVR_ALARMINFO struAlarmInfo;
	memcpy(&struAlarmInfo, pAlarmInfo, sizeof(NET_DVR_ALARMINFO));
	switch(lCommand) 
	{       
	case COMM_ALARM:
		{
			switch (struAlarmInfo.dwAlarmType)
			{
			case 3: //ÒÆ¶¯Õì²â±¨¾¯
               for (i=0; i<16; i++)   //#define MAX_CHANNUM   16  ×î´óÍ¨µÀÊý
               {
                   if (struAlarmInfo.dwChannel[i] == 1)
                   {
                       printf("·¢ÉúÒÆ¶¯Õì²â±¨¾¯µÄÍ¨µÀºÅ£º%d\n", i+1);
                       saveData();
                   }
               }       
			break;
			default:
				break;
          }
		}
	break;
	default:
	break;
  }

}

void TestRebootDVR(LONG lUserID)
{
	if(NET_DVR_RebootDVR(lUserID))
		printf("reboot success!\n");
	else
		printf("Reboot failed!%d\n",NET_DVR_GetLastError());
}


void TestShutdownDVR(LONG lUserID)
{
	if(NET_DVR_ShutDownDVR(lUserID))
		printf("shutdown success!\n");
	else
		printf("shutdown failed!%d\n",NET_DVR_GetLastError());
}

int saveData()
{
	 long lRealPlayHandle;
    NET_DVR_CLIENTINFO ClientInfo = {0};
#if (defined(_WIN32) || defined(_WIN_WCE)) || defined(__APPLE__)
    ClientInfo.hPlayWnd     = NULL;
#elif defined(__linux__)
   ClientInfo.hPlayWnd     = 0;
#endif

    ClientInfo.lChannel     = 1;  //channel NO.
    //ClientInfo.lLinkMode    = 0x40000000; //Record when breaking network.
    ClientInfo.lLinkMode    = 0;
    ClientInfo.sMultiCastIP = NULL;

	printf("Start realplay...\n");
    //getchar();
    //lRealPlayHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo, g_RealDataCallBack_V30, NULL, 0);
    lRealPlayHandle = NET_DVR_RealPlay_V30(lUserID, &ClientInfo,NULL,NULL,0);
    if (lRealPlayHandle < 0)
    {
        printf("pyd1---NET_DVR_RealPlay_V30 error\n");
        return HPR_ERROR;
    }

    //Set rtsp callback function of getting stream.
    //NET_DVR_SetStandardDataCallBack(lRealPlayHandle, g_StdDataCallBack, 0);
    
    printf("Start save data...\n");
    time_t nowtime;
	struct tm *timeinfo;

	time(&nowtime);
	timeinfo = localtime(&nowtime);
	sprintf(fileName,"%d%02d%02d-%02d-%02d-%02d.mp4",1900+timeinfo->tm_year,1+timeinfo->tm_mon,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	
    int iRet,fd;
	fd = open(fileName,O_RDWR|O_CREAT|O_TRUNC|O_APPEND,S_IRWXU|S_IRGRP|S_IROTH);
    close(fd);
	iRet = NET_DVR_SaveRealData(lRealPlayHandle,fileName);
    if(!iRet)
    {
		printf("pyd1---NET_DVR_SaveRealData error:%s\n",NET_DVR_GetLastError());
        NET_DVR_StopSaveRealData(lRealPlayHandle);
        NET_DVR_Logout(lUserID);
        NET_DVR_Cleanup();  
        return 1;
	}
	//getchar();
/*
#ifdef _WIN32
    Sleep(5000);  //millisecond
#elif  defined(__linux__) ||defined(__APPLE__)
    sleep(30);    //second
#endif
*/
	while(saveFlag)
		wait(NULL);
	printf("save data success!\n");
    //stop		
    NET_DVR_StopSaveRealData(lRealPlayHandle);
    //NET_DVR_StopRealPlay(lRealPlayHandle);
	//NET_DVR_Logout(lUserID);
	//NET_DVR_Logout_V30(lUserID);
	//cleanup
	//NET_DVR_Cleanup();
   
    return HPR_OK;
}

int Demo_Alarm()
{
//     if (Demo_AlarmListen() == HPR_ERROR)
//     {
//         return HPR_ERROR;
//     }

	if (Demo_AlarmFortify() == HPR_ERROR)
	{
		return HPR_ERROR;
	}

    return HPR_OK;
}

int Demo_AlarmFortify()
{
	//ÉèÖÃ±¨¾¯»Øµ÷º¯Êý
	NET_DVR_SetDVRMessageCallBack_V30(MessageCallback, NULL);
  
	//ÆôÓÃ²¼·À
	LONG lHandle;
	lHandle = NET_DVR_SetupAlarmChan_V30(lUserID);
	if (lHandle < 0)
	{
		printf("NET_DVR_SetupAlarmChan_V30 error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
	}
  
	sleep(5000);
	//³·Ïú²¼·ÀÉÏ´«Í¨µÀ
	if (!NET_DVR_CloseAlarmChan_V30(lHandle))
	{
		printf("NET_DVR_CloseAlarmChan_V30 error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Logout(lUserID);
		NET_DVR_Cleanup(); 
		return HPR_ERROR;
	}
	saveFlag = 0;
	//×¢ÏúÉè±¸
	NET_DVR_Logout(lUserID);
	//ÊÍ·ÅSDK×ÊÔ´
	NET_DVR_Cleanup();
	return HPR_OK;
}
int cgiMain(void)
{

	//初始化
	NET_DVR_Init();

	NET_DVR_SetLogToFile(3,"./sdkLog");

	//设置链接时间与重连时间
	NET_DVR_SetConnectTime(2000,1);
	NET_DVR_SetReconnect(10000,true);

	//注册设备
	LONG lUserID;
	NET_DVR_DEVICEINFO_V30 struDevice;
	lUserID = NET_DVR_Login_V30("192.168.1.64",8000,"admin","12345",&struDevice);
	if(lUserID < 0)
	{
		printf("Login in ERROR:%d\n",NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return 1;
	}
	printf("login success!\n");

	cgiHeaderContentType("Text/html");
	//while(1){
		if(cgiFormSubmitClicked("reboot") == cgiFormSuccess)
		{
			TestRebootDVR(lUserID);
		}

		if(cgiFormSubmitClicked("Alarm") == cgiFormSuccess)
		{
			if(Demo_Alarm() == HPR_ERROR)
				goto EXIT;
		}
	//}
	
EXIT:
	NET_DVR_Logout_V30(lUserID);
	NET_DVR_Cleanup();

	return 0;
}
