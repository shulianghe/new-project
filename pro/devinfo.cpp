#include <stdio.h>
#include <HCNetSDK.h>
#include <unistd.h>

int main()
{

	if(!NET_DVR_Init())
	{
		printf("INIT ERROR");
		return -1;
	}
	printf("init success!\n");

	NET_DVR_SetConnectTime(2000,1);
	NET_DVR_SetReconnect(1000,true);

	LONG lUserID;
	NET_DVR_DEVICEINFO_V30 devInfo;
	lUserID = NET_DVR_Login_V30("192.168.1.64",8000,"admin","12345",&devInfo);

	if(lUserID < 0)
	{
		printf("login error!%d\n",NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return -1;
	}
	
	printf("login in success!\n g_lUserID = %d\n"
			"devInfo.sSerialNumber[%s],\n"
			"devInfo.byAlarmInPortNum[%d],\n"
			"devInfo.byAlarmOutPortNum[%d],\n"
			"devInfo.byDiskNum[%d],\n"
			"devInfo.byDVRType[%d],\n"
			"devInfo.byChanNum[%d],\n"
			"devInfo.byStartChan[%d].\n",
			 lUserID,
			 devInfo.sSerialNumber,
			 devInfo.byAlarmInPortNum,
			 devInfo.byAlarmOutPortNum,
			 devInfo.byDiskNum,
			 devInfo.byDVRType,
			 devInfo.byChanNum,
			 devInfo.byStartChan);

	NET_DVR_Logout(lUserID);
	NET_DVR_Cleanup();
	return 0;
}
