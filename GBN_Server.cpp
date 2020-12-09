#include<iostream>
#include<string>
#include<winsock.h>
#include<thread>
#include <fstream> 

#pragma comment(lib,"ws2_32.lib")
using namespace std;


//数据包一共1500个字节
//0-4表示序列号，5-9表示确认序列号
//10-14是标志位，分别保留位、A（ack）、R（reset）、S（syn）、F（fin）
//15-19是校验和，20-1499是数据
char message[1024];
char recvBuf[1024];


//套接字初始化
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


int check(char* ch)
{
	int sum = 0;
	for (int i = 0; i < 1024; i++)
	{
		if (i >= 15 && i <= 19) { continue; }
		sum += abs((int)ch[i]) % 10;
	}
	return sum;
}


int print()
{
	if (message[11] == '1')
	{
		cout << "ack";
	}
	if (message[13] == '1')
	{
		cout << " " << "syn" << endl;
	}
	else if (recvBuf[11] == '1' && recvBuf[14] == '1')
	{
		cout << " " << "fin" << endl;
		return 1;
	}
	else
	{
		cout << endl;
	}
	return 0;
}


void mywrite(const char* path)
{
	//message是一个字符数组，13位表示的是syn表示建立连接
	//14位表示的是fin，表示的是关闭连接
	//10位是长度位，用来判断实际长度是否为1500个字节
	if (recvBuf[13] != '1' && recvBuf[14] != '1' && recvBuf[10] != '1')
	{
		ofstream ofile(path, ios::app | ios::binary | ios::out);
		ofile.write(recvBuf + 25, 999);
	}
	else if (recvBuf[13] != '1' && recvBuf[14] != '1' && recvBuf[10] == '1')
	{
		ofstream ofile(path, ios::app | ios::binary | ios::out);
		//如果不是1500个字节，则需要从数据包的最后5位中读出来实际长度，并写入
		ofile.write(recvBuf + 25, char_to_num(20, 24, recvBuf));
	}
}


void mysend(SOCKET sockServer, SOCKADDR_IN addrClient, int nAddrlen)
{
	if (recvBuf[13] == '1')
	{
		num_to_char(13, 13, 1);
		num_to_char(11, 11, 1);
	}
	if (recvBuf[11] == '1' && recvBuf[14] == '1')
	{
		num_to_char(11, 11, 1);
		num_to_char(14, 14, 1);
	}
	num_to_char(11, 11, 1);
	num_to_char(0, 4, 1);
	num_to_char(15, 19, check(message));

	sendto(sockServer, message, 1024, 0, (SOCKADDR*)&addrClient, nAddrlen);
}


int main()
{
	inits();
	initc();

	SOCKET sockServer = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(1234);
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "端口绑定失败！" << endl;
		return -1;
	}

	SOCKADDR_IN addrClient;
	int nAddrlen = sizeof(addrClient);

	int exp_seq = 0;


	cout << "receiving" << endl;
	for (int i = 0;; i++)
	{
		int ret = recvfrom(sockServer, recvBuf, 1024, 0, (SOCKADDR*)&addrClient, &nAddrlen);
		if (ret == 0)
		{
			cout << "没有收到" << endl;
			return -1;
		}
		else
		{
			recvBuf[1024] = '\0';

			int seq = char_to_num(0, 4, recvBuf);
			int exp = char_to_num(5, 9, recvBuf);
			int che = char_to_num(15, 19, recvBuf);
			initc();
			if (che != check(recvBuf))
			{
				cout << "校验和不对，请重传" << endl;
				num_to_char(5, 9, seq);
			}
			else
			{
				if (recvBuf[13] == '1')
				{
					exp_seq = seq;
				}
				else
				{
					//通过自己维护的期望序列号和收到的数据包的序列号进行比较
					if (seq == exp_seq)
					{
						exp_seq++;
						num_to_char(5, 9, exp_seq);
						cout << "已接收" << seq << "号数据包" << endl;


						//通过数据包的Type位判断写入的格式是jpg还是txt
						if (char_to_num(12, 12, recvBuf) == 0)
						{
							const char* path = "D:\\4.jpg";
							mywrite(path);
						}
						else if (char_to_num(12, 12, recvBuf) == 1)
						{
							const char* path = "D:\\output.txt";
							mywrite(path);
						}
					}
				}
			}

			mysend(sockServer, addrClient, nAddrlen);
			

			//用来测试超时重传，即10个数据包之后不再发送ack
			//if (i < 10)
			//{
			//	mysend(sockServer, addrClient, nAddrlen);
			//}

			//用来测试快速重传，回复的ack都会是针对同一个数据包的ack
			//if (i > 10)
			//{
			//	num_to_char(5, 9, 9);
			//	mysend(sockServer, addrClient, nAddrlen);
			//}
			//else
			//{
			//	mysend(sockServer, addrClient, nAddrlen);
			//}

			int ret = print();
			if (ret == 1)
			{
				break;
			}

		}
	}
	closesocket(sockServer);
	WSACleanup();
	return(0);
}