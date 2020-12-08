#include <time.h>
#include <winsock.h>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

//数据包一共1500个字节
//0-4表示序列号，5-9表示确认序列号
//10-14是标志位，分别保留位、A（ack）、T（type）、S（syn）、F（fin）
//保留位在判断是否是最后一个包的时候用到了，类型表示发送的是文字还是图片，便于接收方进行存储
//15-19是校验和，20-24是长度位，在最后一个包的时候使用
//25-1043是数据
char message[1024];
char recvBuf[1024];
int n, countack;
string path;



//初始化套接字
void inits()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		cout << "没有正确初始化socket" << endl;
		return;
	}
}
			  

void initc()
{
	for (int i = 0; i < 1024; i++)
	{
		message[i] = '0';
	}
	message[1023] = '\0';
}


void num_to_char(int start, int end, int n)
{
	string str = to_string(n);
	int j = str.length();
	for (int i = end; i >= start && j >= 1; i--)
	{
		message[i] = str[j - 1];
		j--;
	}
}


int char_to_num(int start, int end, char* ch)
{
	string str;
	for (int i = start; i <= end; i++)
	{
		str += ch[i];
	}
	int n = atoi(str.c_str());
	return n;
}


//偏移量的得到通过读取次数*1480得到，1480是数据包数据区的固定大小
int read(int offset, const char* p)
{
	//表示读取的文件是jpg格式的，需要将数据包中状态字的Type位置位
	if (p[strlen(p) - 1] == 'g')
	{
		num_to_char(12, 12, 0);
	}
	//Type位置位表示读取的文件是txt格式的
	else if (p[strlen(p) - 1] == 't')
	{
		num_to_char(12, 12, 1);
	}


	ifstream infile;
	infile.open(p, ios::in | ios::binary);
	infile.seekg(0, infile.end);
	//整个文件大小的获取
	int length = infile.tellg();
	//偏移量大于文件长度表示文件已经读取完毕，可以直接推出
	if (offset > length) { return 1; }
	infile.seekg(offset);
	if (!infile.is_open()) return -1;

	//剩余的长度不够1480个字节就只读取剩余长度的，并且将状态字中的长度位置位
	//并且在数据区中存储对应实际长度，便于接收端的存储
	if (length - offset < 999)
	{
		char* buf = new char[length - offset + 1];
		infile.read(buf, length - offset);
		buf[length - offset] = '\0';
		num_to_char(10, 10, 1);
		for (int i = 0; i < length - offset; i++)
		{
			message[25 + i] = buf[i];
		}
		num_to_char(20, 24, length - offset);
	}
	else
	{
		char buf[1000];
		infile.read(buf, 999);
		buf[999] = '\0';

		for (int i = 0; i < 999; i++)
		{
			message[25 + i] = buf[i];
		}
	}
	return 0;
}


//参数ch可以是发送时候的数据包，也可以是接收到的ack的数据包也需要进行校验
int check(char* ch)
{
	int sum = 0;
	//将数组从头至尾遍历
	for (int i = 0; i < 1024; i++)
	{
		//计算校验和的时候，保存校验和的字段不会进行计算
		if (i >= 15 && i <= 19) { continue; }
		//为了减小计算结果，将操作数变为ascii%10后的数据
		sum += abs((int)ch[i]) % 10;
	}
	return sum;
}


void myrecv(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen, int& flag, int& base, int& countack)
{
	int ret = recvfrom(sockClient, recvBuf, 1024, 0, (SOCKADDR*)&addrServer, &SerAddrlen);
	if (ret < 0)
	{
		cout << "服务器端没有打开连接，请等待一段时间再试" << endl;
		return;
	}
	int exp = char_to_num(5, 9, recvBuf);
	if (exp > base)
	{
		flag = 1;
		base = exp;
		countack = 0;
	}
	else if (exp == base)
	{
		flag = 0;
		countack++;
	}
	return;
}


void start_timer(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen, int& nextseqnum, int& base, int &flag, int &countack)
{
	thread t1(myrecv, sockClient, addrServer, SerAddrlen, ref(flag), ref(base), ref(countack));


	clock_t now = clock();
	while (1) {
		if (flag == 1)
		{
			t1.detach();
			return;
		}
		if (countack == 3)
		{
			nextseqnum = base;
			t1.detach();
			cout << "已快速重传" << base << "号数据包" << endl;
			return;
		}
		if (((double)clock() - (double)now) >= 1000)
		{
			nextseqnum = base;
			t1.detach();
			cout << "已超时重传" << base << "号数据包" << endl;
			return;
		}
	}
}


int shake(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen)
{
	num_to_char(0, 4, 0);
	num_to_char(5, 9, 0);
	num_to_char(13, 13, 1);
	num_to_char(15, 19, check(message));

	sendto(sockClient, message, 1024, 0, (SOCKADDR*)&addrServer, SerAddrlen);
	int ret = recvfrom(sockClient, recvBuf, 1024, 0, (SOCKADDR*)&addrServer, &SerAddrlen);

	if (ret < 0)
	{
		cout << "对方未打开服务器，请稍后再试" << endl;
		return -1;
	}
	if (recvBuf[11] == '1' && recvBuf[13] == '1')
	{
		cout << "已成功建立连接" << endl;
	}

	return 0;
}


int transfer(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen, int seq, const char* p)
{

	initc();
	num_to_char(0, 4, seq);
	num_to_char(5, 9, 1);
	num_to_char(10, 14, 0);
	num_to_char(15, 19, 0);
	if (read(seq * 999, p) == 1) { return 1; }
	num_to_char(15, 19, check(message));

	sendto(sockClient, message, 1024, 0, (SOCKADDR*)&addrServer, SerAddrlen);
	cout << "已发送" << seq << "号数据包" << endl;

	return 0;
}


int close(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen)
{
	initc();
	num_to_char(0, 4, 0);
	num_to_char(5, 9, 0);
	num_to_char(11, 11, 1);
	num_to_char(14, 14, 1);
	num_to_char(15, 19, check(message));
	sendto(sockClient, message, 1024, 0, (SOCKADDR*)&addrServer, SerAddrlen);
	cout << "已发送结束数据包" << endl;
	int ret = recvfrom(sockClient, recvBuf, 1024, 0, (SOCKADDR*)&addrServer, &SerAddrlen);
	if (ret < 0)
	{
		cout << "error" << endl;
		return -1;
	}
	closesocket(sockClient);
	WSACleanup();
}


//主函数
int main(int argc, char* argv[])
{
	inits();
	initc();


	SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);

	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(1234);
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int SerAddrlen = sizeof(addrServer);

	cout << "请输入你想发送的文件名称：";
	cin >> path;
	path = "D:\\" + path;
	const char* p = path.c_str();


	cout << "请输入滑动窗口大小：";
	cin >> n;

	int ret;
	ret = shake(sockClient, addrServer, SerAddrlen);
	if (ret == -1)
	{
		return 0;
	}


	int base = 0, nextseqnum = 0, flag = 0, countack = 0;
	while (1)
	{
		if (nextseqnum < base + n)
		{
			int ret = transfer(sockClient, addrServer, SerAddrlen, nextseqnum, p);
			if (ret == 1)
				break;
			nextseqnum++;

			flag = 0;
			countack = 0;
			
			thread t2(start_timer, sockClient, addrServer, SerAddrlen, ref(nextseqnum), ref(base), ref(flag), ref(countack));
			t2.detach();
			

		}
	}

	ret = close(sockClient, addrServer, SerAddrlen);
	if(ret==-1)
	{
		return 0;
	}

	return 0;
}