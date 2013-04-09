#########################################################################
# File Name: namp.sh
# Author: zyb
# mail: nevergdy@gmail.com
# Created Time: 2013年02月19日 星期二 10时47分52秒
#########################################################################
#!/bin/bash
#nmap.sh:嗅探局域网内活跃ip并将他们写入iplist.txt

#按语言得不同选择
ip=`ifconfig eth0 |grep 'inet 地址'|awk -F '  *|:' '{print $4}'`
#ip=`ifconfig eth0 |grep 'inet addr'|awk -F '  *|:' '{print $4}'`
#echo "Host ip = $ip"

#nmap -sP "$ip"/24 >tmp.txt
nmap "$ip"/24 -p 554 >tmp.txt
#echo `cat tmp.txt`

#显示符合的端口地址的上两行得到需要的IP信息
grep -B 3 open tmp.txt >iplist.txt
#grep -B 2 00:0C:29 tmp.txt >iplist.txt
#echo;echo `cat iplist.txt`
deviceIP=`grep -Eo '([0-9]{3}).([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3})' iplist.txt`
#deviceMACAdd=`grep 'MAC Address' iplist.txt | awk '{print $3}'`

echo "device ip is $deviceIP"
#echo;echo "device MAC address is $deviceMACAdd"

rm tmp.txt

sqlite3 device.db "create table devicelist(deviceName text,deviceIP VARCHAR(100))"

sqlite3 device.db "select * from devicelist" > existIP.txt
existIP=`grep -Eo '([0-9]{3}).([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3})' existIP.txt`
rm existIP.txt

name=camera
i=1
#for ip in $deviceIP
#do
#	sqlite3 device.db "insert into devicelist values('$name$i','$ip')"
#	((i++))
#	echo $ip
#done
flag=0
for newIP in $deviceIP
do
	flag=0
	for oldIP in $existIP
	do
		if [ "$newIP"x = "$oldIP"x ]; then
			echo "IP '$newIP' already exist"
			((i++))
            flag=1
			break
		fi
        ((i++))
	done
	if [ $flag -eq 0 ];then
		sqlite3 device.db "insert into devicelist values('$name$i','$newIP')"
	    ((i++))
    fi 
#	((i++))
#	echo $newIP 
done

sqlite3 device.db "select * from devicelist"                  

