#########################################################################
# File Name: namp_arm.sh
# Author: zyb
# mail: nevergdy@gmail.com
# Created Time: 2013年02月20日 星期二 10时47分52秒
#########################################################################
#!/bin/bash
#nmap.sh:嗅探局域网内活跃ip并将他们写入iplist.txt
#用于ARM

#按语言得不同选择
#ip=`ifconfig eth0 |grep 'inet 地址'|awk -F '  *|:' '{print $4}'`
ip=`ifconfig eth0 |grep 'inet addr'|awk -F '  *|:' '{print $4}'`
echo "Host ip = $ip"

nmap -sP "$ip"/24 >tmp.txt
#echo `grep '64' tmp.txt`

#显示符合的MAC地址的上两行得到需要的IP信息
grep -B 1 A0:E9:DB tmp.txt >iplist.txt
echo;echo `cat iplist.txt`
deviceIP=`grep -Eo '([0-9]{3}).([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3})' iplist.txt`
deviceMACAdd=`grep 'MAC Address' iplist.txt | awk '{print $3}'`

echo;echo "device ip is $deviceIP"
echo;echo "device MAC address is $deviceMACAdd"

rm tmp.txt

