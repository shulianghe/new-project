/*************************************************************************
 > File Name: devMan.c
 > Author: zyb
 > Mail:nevergdy@gmail.com 
 > Created Time: 2013年03月12日 星期二 14时09分32秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "cgic.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "cJSON.h"

#define FILENAME "dev.ashx"

/* Used by some code below as an example datatype. */
struct record {const char *deviceName;const char *IP; };

cJSON* dofile(char *filename)
{
	cJSON *json;
	FILE *f=fopen(filename,"rb");
	if(f)
	{
		fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
		char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
		json=cJSON_Parse(data);
		free(data);
		return json;
	}
	else
	return NULL;
}

/* Create a bunch of objects as demonstration. */
int create_objects(FILE *fp,char *name,char *ip)
{
	cJSON *root,*dev,*devs;char *out;int i;	// declare a few. 
	//Here we construct some JSON standards, from the JSON site. 
	
	
	struct record device={name,ip};

	root=cJSON_CreateArray();
	//for (i=0;i<2;i++)
	//{
		cJSON_AddItemToArray(root,devs=cJSON_CreateObject());
		cJSON_AddStringToObject(devs, "deviceName", device.deviceName);
		cJSON_AddStringToObject(devs, "IP", device.IP);
	//}
	
//	cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root,1),"City",cJSON_CreateIntArray(ids,4)); 
//	printf("out is \n");
	out=cJSON_Print(root);
	
		
	fprintf(fp,"%s",out);
	cJSON_Delete(root);	printf("%s\n",out);	free(out);
	fclose(fp);
	
	return 0;
}

/* Read a file, parse, render back, add item ,etc. */
int addToFile(char *filename,char *name,char *ip)
{
	FILE *f;
	char *out;cJSON *json,*item,*devs,*s1;
	int i;
	struct record dev={name,ip};

	json=dofile(filename);
	
	FILE *fp = fopen(FILENAME,"w");
	//if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());return 1;}
	if(!json)
	{
		if(NULL == cJSON_GetErrorPtr())
		{
			printf("FILE NOT EXIST\n");
			if(!create_objects(fp,name,ip))
				return 0;
			return 1;
		}
		fclose(fp);
		return 1;
	}
	else
	{/*
		//检查IP是否已经存在
		s1 = json->child;
		while(s1){
			//i++;
			printf("device's Name is %s\n",cJSON_GetObjectItem(s1,"deviceName")->valuestring);
			printf("device's IP is %s\n",cJSON_GetObjectItem(s1,"IP")->valuestring);
			
			if(!strcmp(cJSON_GetObjectItem(s1,"deviceName")->valuestring,name) | !strcmp(cJSON_GetObjectItem(s1,"IP")->valuestring,ip))
			{
				printf("This(name=%s,IP=%s) device already exist,name or ip repeated.Please check and retry.",cJSON_GetObjectItem(s1,"deviceName")->valuestring,cJSON_GetObjectItem(s1,"IP")->valuestring);
				return 1;
			}
			s1 = s1->next;
		}*/
		cJSON_AddItemToArray(json,devs=cJSON_CreateObject());
		cJSON_AddStringToObject(devs, "deviceName", dev.deviceName);
		cJSON_AddStringToObject(devs, "IP", dev.IP);
		out=cJSON_Print(json);
		cJSON_Delete(json);
		fprintf(fp,"%s",out);
		free(out);
	}
	//free(data);
	fclose(fp);
	
	return 0;
}

//delete item from dev.ashx
int delItem(char *filename,char *name,char *ip)
{
	char *out,*s;cJSON *json,*s1;
	int i=0;
	
	json=dofile(filename);
	
	FILE *fp = fopen(FILENAME,"w");
	if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());return 1;}
	else
	{
		s1 = json->child;
		while(s1){
			//遍历文件内容
		//	printf("device's Name is %s\n",cJSON_GetObjectItem(s1,"deviceName")->valuestring);
		//	printf("device's IP is %s\n",cJSON_GetObjectItem(s1,"IP")->valuestring);
			//以IP来判断要删除的设备,找到要删除的IP跳出循环
			if(!strcmp(cJSON_GetObjectItem(s1,"IP")->valuestring,ip))
			{
		//		printf("del device's Name is %s\n",cJSON_GetObjectItem(s1,"deviceName")->valuestring);
		//		printf("del device's IP is %s\n",cJSON_GetObjectItem(s1,"IP")->valuestring);
		//		printf("i=%d\n",i);
				break;
			}
			i++;
			s1 = s1->next;
		}
		//IP不存在的情况
		if(i==cJSON_GetArraySize(json))
		{
			printf("IP(%s) you want delete is not exist,Please check and retry...\n",ip);
			out=cJSON_Print(json);
			cJSON_Delete(json);
			//printf("in del out = %s\n",out);
			fprintf(fp,"%s",out);
			free(out);
			fclose(fp);
			return 1;
		}
		cJSON_DeleteItemFromArray(json,i);
		out=cJSON_Print(json);
		cJSON_Delete(json);
		//printf("in del out = %s\n",out);
		fprintf(fp,"%s",out);
		free(out);
	}
	fclose(fp);
	
	return 0;
}

int checkIP(const char *str)
{
    struct in_addr addr;
    int ret;
    volatile int local_errno;

    //errno = 0;
    ret = inet_pton(AF_INET, str, &addr);
    local_errno = errno;
    /*
    if (ret > 0)
        fprintf(stderr, "\"%s\" is a valid IPv4 address\n", str);
    else if (ret < 0)
        fprintf(stderr, "EAFNOSUPPORT: %s\n", strerror(local_errno));
    else
        fprintf(stderr, "\"%s\" is not a valid IPv4 address\n", str);
	*/
    return ret;
}

int cgiMain()
{
	char name[20]="";
	char IP[20]="";
	cgiHeaderContentType("Text/html");
	if(cgiFormString("name",name,sizeof(name)) == cgiFormSuccess)
	{
		printf("name = %s\n",name);
	}
	if(cgiFormString("ip",IP,sizeof(IP)) == cgiFormSuccess)
	{
		printf("ip = %s\n",IP);
	}
	if(checkIP(IP)>0){
		if(cgiFormSubmitClicked("add") == cgiFormSuccess)
		{
			if(addToFile(FILENAME,name,IP))
			{
				printf("add error!");
				return 1;
			}
			printf("Device add success!Please refresh video page to get newest device list.\n");
		}
		if(cgiFormSubmitClicked("delete") == cgiFormSuccess)
		{
			if(delItem(FILENAME,name,IP))
			{
				printf("delete error!");
				return 1;
			}
			printf("Device delete success!Please refresh video page to get newest device list.\n");
		}
	}
	else
		printf("INVALID IP!!!Please check IP and try again...\n");
		
	return 0;
}


