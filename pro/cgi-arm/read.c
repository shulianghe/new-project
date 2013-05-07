/*************************************************************************
 > File Name: read.c
 > Author: zyb
 > Mail:nevergdy@gmail.com 
 > Created Time: 2013年04月25日 星期四 16时02分05秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main()
{
    char *ip,*file = "/tmp/cameraIP.txt";
    int fd,len = 32;

    ip = calloc(len,sizeof(char));
    if(ip == NULL)
    {
        perror("calloc");
        return 1;
    }

    fd  = open(file,O_RDONLY,0644);
    if(fd == -1)
    {
        perror("open");
        return 1;
    }

    read(fd,ip,len);

    printf("ip is %s\n",ip);
    close(fd);
    return 0;
}
