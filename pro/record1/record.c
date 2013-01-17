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
	riffheader.szRiffID[0]='R';
	riffheader.szRiffID[1]='I';
	riffheader.szRiffID[2]='F';
	riffheader.szRiffID[3]='F';
	riffheader.dwRiffSize=1024*50+8+16+8+4;
	riffheader.szRiffFormat[0]='W';
	riffheader.szRiffFormat[1]='A';
	riffheader.szRiffFormat[2]='V';
	riffheader.szRiffFormat[3]='E';

	fmtblock.szFmtID[0]='f';
	fmtblock.szFmtID[1]='m';
	fmtblock.szFmtID[2]='t';
	fmtblock.szFmtID[3]=' ';
	fmtblock.dwFmtSize=16;
	fmtblock.wavFormat.wFormatTag=0x0001;
	fmtblock.wavFormat.wChannels=1;
	fmtblock.wavFormat.dwSamplesPerSec=8000;
	fmtblock.wavFormat.dwAvgBytesPerSec= 8000*2;////////////////////
	fmtblock.wavFormat.wBlockAlign=2;
	fmtblock.wavFormat.wBitsPerSample=16;

	datablock.szDataID[0]='d';
	datablock.szDataID[1]='a';
	datablock.szDataID[2]='t';
	datablock.szDataID[3]='a';

	datablock.dwDataSize=1024*50;


	if((fp=fopen("test.wav","wb")) == NULL)
	{
		printf("Cannot open test.wav");
		return;
	}
	fwrite(&riffheader,sizeof(riffheader),1,fp);
	fwrite(&fmtblock,sizeof(fmtblock),1,fp);
	fwrite(&datablock,sizeof(datablock),1,fp);


	if(!OpenSnd())
	{
		printf("Open sound device error!\\n");
		exit(-1);
	}
	SetFormat(FMT16BITS, FMT8K);
	SetChannel(MONO);
	buf = (char *)malloc(1024);
	if(buf == NULL)
		exit(-1);
	for(i = 0; i <50; i++)
	{
		printf("%d \n",i);
		dwSize = Record(buf, 1024);
		fwrite(buf,dwSize,1,fp);
		//dwSize = Play(buf, dwSize);
	}
	fclose(fp);
	exit(1);
}
