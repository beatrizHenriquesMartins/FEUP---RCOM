#!/bin/bash
hostname=$(hostname | tr -d 'tux')
stand=$(echo $hostname | head -c 1)

if [ "`echo $HOSTNAME|grep tux${stand}1 -c`" = "1" ]; then
	/etc/init.d/networking restart
	arp -d ipaddress
	ifconfig eth0 down
	ifconfig eth0 up
	ifconfig eth0 172.16.${stand}0.1/24
	route add -net 172.16.${stand}1.0/24 gw 172.16.${stand}0.254
	route add default gw 172.16.${stand}0.254
	echo -e "search netlab.fe.up.pt\nnameserver 172.16.1.1\n" > /etc/resolv.conf
	ifconfig
fi
if [ "`echo $HOSTNAME|grep tux${stand}4 -c`" = "1" ]; then
	arp -d ipaddress
	ifconfig eth0 down
	ifconfig eth1 down
	ifconfig eth1 up
	ifconfig eth0 up
	ifconfig eth0 172.16.${stand}0.254/24
	ifconfig eth1 172.16.${stand}1.253/24
	route add default gw 172.16.${stand}1.254
	echo 1 > /proc/sys/net/ipv4/ip_forward
	echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
	echo -e "search netlab.fe.up.pt\nnameserver 172.16.1.1\n" > /etc/resolv.conf
	ifconfig
fi
if [ "`echo $HOSTNAME|grep tux${stand}2 -c`" = "1" ]; then
	arp -d ipaddress
	ifconfig eth0 down
	ifconfig eth0 up
	ifconfig eth0 172.16.${stand}1.1/24
	route add -net 172.16.${stand}0.0/24 gw 172.16.${stand}1.253
	route add default gw 172.16.${stand}1.254
	echo -e "search netlab.fe.up.pt\nnameserver 172.16.1.1\n" > /etc/resolv.conf
	ifconfig
fi
route -n

