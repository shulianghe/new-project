#########################################################################
# File Name: namp.sh
# Author: zyb
# mail: nevergdy@gmail.com
# Created Time: 2013年02月19日 星期二 10时47分52秒
#########################################################################
#!/bin/bash
#nmap.sh:嗅探局域网内活跃ip并将他们写入iplist.txt

#判断wifi热点是否开启，若开启则以br0扫描，否则eth0扫描,wifi热点优先
CONFIG_FILE=/opt/homewell/nwcfg/minidb.xml

WIFI_MODE=`cat $CONFIG_FILE |sed -n '/<Wifi>/,/<\/Wifi>/p' |grep "<Mode>" |sed 's/^.*<Mode>//' |sed 's/<\/Mode>.*$//'`
if [ $? != "0" ];then exit 1;fi
#test $WIFI_MODE != wifiAPMode && exit 0
if [ "$WIFI_MODE"x = "wifiAPMode"x ]
then
	ip=`ifconfig br0 |grep 'inet addr'|awk -F '  *|:' '{print $4}'`
else
	ip=`ifconfig eth0 |grep 'inet addr'|awk -F '  *|:' '{print $4}'`
fi

#按语言得不同选择
#ip=`ifconfig eth0 |grep 'inet 地址'|awk -F '  *|:' '{print $4}'`
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

#echo "device ip is $deviceIP"
#echo;echo "device MAC address is $deviceMACAdd"

rm tmp.txt

#dir=`pwd`
dir=/opt/homewell/vwlite
#echo $dir

sqlite3 $dir/device.db "create table devicelist(deviceName text,deviceIP VARCHAR(100),proxyState integer DEFAULT 0,clientNum integer DEFAULT 0, rtspUrl text DEFAULT NULL)"

sqlite3 $dir/device.db "select * from devicelist" > existIP.txt
existIP=`grep -Eo '([0-9]{3}).([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3})' existIP.txt`
rm existIP.txt

name=camera
i=1
#for ip in $deviceIP
#do
#	sqlite3 $dir/device.db "insert into devicelist(deviceName,deviceIP) values('$name$i','$ip')"
#	((i++))
#	echo $ip
#done

#删除未扫描到的设备信息
flag=0
for oldIP in $existIP
do
	flag=0
	for newIP in $deviceIP
	do
		if [ "$oldIP"x == "$newIP"x ]; then
#			sqlite3 $dir/device.db "update devicelist set proxyState=0,rtspUrl='' where deviceIP='$oldIP'" 
			flag=1
			break
		fi
	done
	if [ $flag -eq 0 ];then
#		echo "ip '$oldIP' not exist"
		sqlite3 $dir/device.db "delete from devicelist where deviceIP='$oldIP'"
    	fi 
done
#sqlite3 $dir/device.db "select * from devicelist"

sqlite3 $dir/device.db "select * from devicelist" > existIP.txt
existIP=`grep -Eo '([0-9]{3}).([0-9]{1,3}).([0-9]{1,3}).([0-9]{1,3})' existIP.txt`
rm existIP.txt

#添加扫描到而数据库中没有的设备信息
flag=0
for newIP in $deviceIP
do
	flag=0
	for oldIP in $existIP
	do
		if [ "$newIP"x = "$oldIP"x ]; then
#			echo "IP '$newIP' already exist"
			((i++))
            		flag=1
			break
		fi
        ((i++))
	done
	if [ $flag -eq 0 ];then
		sqlite3 $dir/device.db "insert into devicelist(deviceName,deviceIP) values('$name$i','$newIP')"
	    ((i++))
    fi 
#	((i++))
#	echo $newIP 
done

#sqlite3 $dir/device.db "select * from devicelist"                  

