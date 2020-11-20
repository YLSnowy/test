#include <iostream>
#include <thread>
#include <time.h>
#include <math.h>
#include <typeinfo>
#include "pcap.h"
#include "windows.h"
using namespace std;

struct FrameHeader_t
{
	u_char  DestMAC[6];
	u_char  SrcMAC[6];
	u_short FrameType;
};

struct ARPHeader_t
{
	unsigned short HardwareType;
	unsigned short ProtocolType;
	unsigned char  HLen;
	unsigned char  PLen;
	unsigned short Operation;
	u_char Sendha[6];
	u_char SendIP[4];
	u_char RecvHa[6];
	u_char RecvIP[4];
};

struct ArpPacket {
	FrameHeader_t FrameHeader;
	ARPHeader_t   ARPHeader;
};



unsigned char sendbuf[42];
unsigned char mac[6] = { 0x70,0xC9,0x4E,0xE3,0xBE,0x33 };
unsigned char mac1[6] = { 0xff,0xff,0xff,0xff,0xff,0xff };
unsigned char mac2[6] = { 0x00,0x00,0x00,0x00,0x00,0x00 };


char* errbuf = new char[PCAP_ERRBUF_SIZE];
pcap_if_t* alldevs;
pcap_if_t* d;
pcap_addr_t* a;
pcap_t* p;
char* name;



FrameHeader_t FrameHeader;
ARPHeader_t ARPHeader;



struct pcap_pkthdr* pkt_header;
const u_char* pktdata;
struct tm* ltime;
char timestr[16];
time_t local_tv_sec;

void print(int n)
{
	cout << "     ";
	for (int i = 0; i < n; i++)
	{
		cout << '-';
	}
	cout << endl;
}

int main()
{

	int ret = pcap_findalldevs_ex(const_cast<char*>(PCAP_SRC_IF_STRING), NULL, &alldevs, errbuf);
	if (ret == -1)
	{
		cout << "没有获取到本机设备列表" << endl;
		return -1;
	}

	int i, j;//i是网卡的数量，j是每个网卡中ip地址的数量

	cout << "网卡相关信息如下表所示：" << endl;
	print(83);
	cout << "     |" << "网卡序号   " << "|" << "网卡名字                                                   " << '|' << "网卡地址  " << endl;
	print(83);


	//输出每个网卡中的ip地址及掩码
	for (d = alldevs, i = 0; d != NULL; d = d->next, i++)
	{
		cout << "     |" << i << "          |" << d->name << " |" << d->addresses << endl;
		print(83);
	}



	int n;
	cout << endl << "请输入您想通过哪个接口发送数据包：";
	cin >> n;
	for (d = alldevs, i = 0; d != NULL && i < n; d = d->next, i++);



	p = pcap_open(d->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf);
	if (p == NULL)
	{
		cout << d->name << "没有被成功打开" << endl;
		pcap_freealldevs(alldevs);
		return -1;
	}




	unsigned char ip[4] = { 192,168,43,37 };
	unsigned char ip1[4];

	cout << endl << "请输入你想寻找对应物理地址的ip地址：";
	for (int i = 0; i < 4; i++)
	{
		cin >> ip1[i];
		char c;
		if (i < 3) { cin >> c; }
	}



	memcpy(FrameHeader.DestMAC, mac1, 6);
	memcpy(FrameHeader.SrcMAC, mac, 6);
	memcpy(ARPHeader.Sendha, mac, 6);
	memcpy(ARPHeader.RecvHa, mac1, 6);
	memcpy(ARPHeader.SendIP, ip, 4);
	FrameHeader.FrameType = htons(0x0806);
	ARPHeader.HardwareType = htons(0x0001);
	ARPHeader.ProtocolType = htons(0x0800);
	ARPHeader.HLen = 6;
	ARPHeader.PLen = 4;
	ARPHeader.Operation = htons(0x0001);
	memset(sendbuf, 0, sizeof(sendbuf));
	memcpy(sendbuf, &FrameHeader, sizeof(FrameHeader));
	memcpy(sendbuf + sizeof(FrameHeader), &ARPHeader, sizeof(ARPHeader));



	if (pcap_sendpacket(p, sendbuf, 42) != 0)
	{
		cout << endl << "arp数据包发送失败" << endl;
	}

	/* 释放设备列表 */
	pcap_freealldevs(alldevs);


	int res;
	//if (res = pcap_next_ex(p, &pkt_header, &pktdata) <= 0) { cout << "timeout" << endl; return 0; }
	while ((res = pcap_next_ex(p, &pkt_header, &pktdata)) >= 0)
	{
		if (res == 0) {  continue; }

		ARPHeader_t* arph = (ARPHeader_t*)(pktdata + 14);
		if (ntohs(arph->Operation) != 2) { continue; }

		local_tv_sec = pkt_header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);


		cout << endl;
		print(85);
		printf("      时间      |源IP           |目的IP         |源MAC地址              |目的MAC地址\n");
		print(85);

		printf("      %s\t", timestr);


		//输出源IP
		for (i = 0; i < 3; i++)
		{
			if (i == 0) { printf("|"); }
			printf("%d.", arph->SendIP[i]);
		}
		printf("%d\t", arph->SendIP[3]);

		//输出目的IP
		for (i = 0; i < 3; i++)
		{
			if (i == 0) { printf("|"); }
			printf("%d.", arph->RecvIP[i]);
		}
		printf("%d\t", arph->RecvIP[3]);

		//输出源MAC
		for (i = 0; i < 5; i++)
		{
			if (i == 0) { printf("|"); }
			printf("%02x-", arph->Sendha[i]);
		}
		printf("%02x\t", arph->Sendha[5]);

		//输出目的MAC
		for (i = 0; i < 5; i++)
		{
			if (i == 0) { printf("|"); }
			printf("%02x-", arph->RecvHa[i]);
		}
		printf("%02x\n", arph->RecvHa[5]);

		print(85);
		break;
	}


	return 0;
}
