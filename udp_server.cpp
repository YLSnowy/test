#include<iostream>
#include<string>
#include<winsock.h>
#include<thread>
#include <fstream> 

#pragma comment(lib,"ws2_32.lib")
using namespace std;


//数据包一共1500个字节，0-4表示序列号，5-9表示确认序列号，10-1494都是数据，1495-1499是校验和
char message[1500];


//套接字初始化
void init()
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		cout << "没有正确初始化socket" << endl;
		return;
	}
}


void write(string ch)
{
	ofstream outfile;
	outfile.open("D:\\4.jpg", ios::out | ios::app);
	outfile << ch;
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
	int j = 0;
	for (int i = 0; i < 5; i++)
	{
		if (i < 5 - str.length())
			message[start + i] = '0';
		else
		{
			message[start + i] = str[j];
			j++;
		}
	}
}

int check(char*ch)
{
	int sum = 0;
	for (int i = 0; i < strlen(ch); i++)
	{
		if (i >= 10 && i <= 14) { continue; }
		sum += abs((int)ch[i]) % 10;
	}
	return sum;
}


int main()
{
	init();


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



	cout << "receiving" << endl;
	for (int i = 0;i<1; i++)
	{
		char recvBuf[1501];

		int ret =recvfrom(sockServer, recvBuf, sizeof(recvBuf), 0, (SOCKADDR*)&addrClient, &nAddrlen);
		if (ret == 0)
		{
			cout << "没有收到" << endl;
			return -1;
		}
		else
		{
			recvBuf[1500] = '\0';

			int seq = char_to_num(0, 4, recvBuf);
			int exp = char_to_num(5, 9, recvBuf);
			int che = char_to_num(10, 14, recvBuf);
			if (che != check(recvBuf))
			{
				cout << "校验和不对，请重传" << endl;
				num_to_char(5, 9, seq);
			}
			else
			{
				num_to_char(5, 9, 1 - seq);
				cout << "已接收" << seq << "号数据包" << endl;

				ofstream ofile("D:\\output1.txt", ios::app | ios::binary | ios::out);
				ofile.write(recvBuf + 15, 1485);
			}

			num_to_char(0, 4, 1);
			num_to_char(10, 14, check(message));
			message[15] = 'a';
			message[16] = 'c';
			message[17] = 'k';

			sendto(sockServer, message, 1500, 0, (SOCKADDR*)&addrClient, nAddrlen);
			cout << message[15] << message[16] << message[17] << endl;
		}
	}
	closesocket(sockServer);
	WSACleanup();
	return(0);
}