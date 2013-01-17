#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sndtools.h"
#define WAVOUTDEV FMT8K
typedef unsigned int DWORD;
typedef unsigned short int WORD; 
struct RIFF_HEADER
{
	char szRiffID[4]; // 'R','I','F','F'
	DWORD dwRiffSize;
	char szRiffFormat[4]; // 'W','A','V','E'
};
struct WAVE_FORMAT
{
	WORD wFormatTag;
	WORD wChannels;
	DWORD dwSamplesPerSec;
	DWORD dwAvgBytesPerSec;
	WORD wBlockAlign;
	WORD wBitsPerSample;
};
struct FMT_BLOCK
{
	char szFmtID[4]; // 'f','m','t',' '
	DWORD dwFmtSize;
	struct WAVE_FORMAT wavFormat;
};
struct FACT_BLOCK
{
	char szFactID[4]; // 'f','a','c','t'
	DWORD dwFactSize;
};
struct DATA_BLOCK
{
	char szDataID[4]; // 'd','a','t','a'
	DWORD dwDataSize;
};
int main()
{

	char *buf;
	int dwSize;
	int i;
	struct RIFF_HEADER riffheader;
	struct FMT_BLOCK fmtblock;
	struct DATA_BLOCK datablock; 
	FILE * fp;
	printf("WORD %d \n",sizeof(WORD));
	printf("DWORD %d\n",sizeof(DWORD));

	if((fp=fopen("test.wav","rw")) == NULL)
	{
		printf("Cannot open test.wav");
		return;
	}

	fread(&riffheader,sizeof(riffheader),1,fp);
	fread(&fmtblock,sizeof(fmtblock),1,fp);
	fread(&datablock,sizeof(datablock),1,fp);


	printf("riff size:%d \n",riffheader.dwRiffSize);
	printf("riff format:%s\n",riffheader.szRiffFormat);


	printf("fmtID: %s \n",fmtblock.szFmtID);

	printf("fmtSize %d \n",fmtblock.dwFmtSize);
	printf("fmtFormat %d\n",fmtblock.wavFormat.wFormatTag);
	printf("fmtChannels %d\n",fmtblock.wavFormat.wChannels);
	printf("SamplePerSec %d\n",fmtblock.wavFormat.dwSamplesPerSec);
	printf("AvgBytesPerSec %d\n",fmtblock.wavFormat.dwAvgBytesPerSec);////////////////////
	printf("BlockAlign %d \n", fmtblock.wavFormat.wBlockAlign);//=2;
	printf("BitsPerSample %d\n",fmtblock.wavFormat.wBitsPerSample);//=16;
	/*
	   datablock.szDataID[0]='d';
	   datablock.szDataID[1]='a';
	   datablock.szDataID[2]='t';
	   datablock.szDataID[3]='a';
	   */
	printf("DataSize %d \n",datablock.dwDataSize);

	if(!OpenSnd())
	{
		printf("Open sound device error!\\n");
		exit(-1);
	}
	SetFormat(fmtblock.wavFormat.wBitsPerSample, fmtblock.wavFormat.dwSamplesPerSec);
	SetChannel(fmtblock.wavFormat.wChannels);
	buf = (char *)malloc(1024);
	if(buf == NULL)
		exit(-1);
	while(!feof(fp))
	{
		//printf("%d \n",i);
		//dwSize = Record(buf, 1024);
		dwSize=fread(buf,1024,1,fp);
		usleep(1);
		dwSize = Play(buf, 1024);
	}

	fclose(fp);

}
