#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#define VAR_STATIC
#include "sndtools.h"
int devfd = 0;
/*
* Open Sound device
* Return 1 if success, else return 0.
*/
int OpenSnd(/* add by new version */int nWhich)
{
	if(devfd > 0)
		close(devfd);
	devfd = open("/dev/snd/pcmC0D0c", O_RDWR);
	if(devfd < 0)
		return 0;
	return 1;
}
/*
* Close Sound device
* return 1 if success, else return 0.
*/
int CloseSnd(/* add by new version */int nWhich)
{
	close(devfd);
	devfd = 0;
	return 1;
}
/*
* Set Record an Playback format
* return 1 if success, else return 0.
* bits -- FMT8BITS(8bits), FMT16BITS(16bits)
* hz -- FMT8K(8000HZ), FMT16K(16000HZ), FMT22K(22000HZ), FMT44K(44000HZ)
*/
int SetFormat(int bits, int hz)
{
	int tmp = bits;
	if( -1 == ioctl(devfd, SNDCTL_DSP_SETFMT, &tmp))
	{
#ifdef DEBUG_WARN
		printf("Set fmt to s16_little faile:%d\\n", nWhich);
#endif
		return 0;
	}
	tmp = hz;
	if( -1 == ioctl(devfd, SNDCTL_DSP_SPEED, &tmp))
	{
#ifdef DEBUG_WARN
		printf("Set speed to %d:%d\\n", hz, nWhich);
#endif
		return 0;
	}
	return 1;
}
/*
* Set Sound Card Channel
* return 1 if success, else return 0.
* chn -- MONO, STERO
*/
int SetChannel(int chn)
{
	int tmp = chn;
	if(-1 == ioctl(devfd, SNDCTL_DSP_CHANNELS, &tmp))
	{
#ifdef DEBUG_WARN
		printf("Set Audio Channel faile:%d\\n", nWhich);
#endif
		return 0;
	}
	return 1;
}
/*
* Record
* return numbers of byte for read.
*/
int Record(char *buf, int size)
{
	return read(devfd, buf, size);
}
/*
* Playback
* return numbers of byte for write.
*/
int Play(char *buf, int size)
{
	return write(devfd, buf, size);
}
