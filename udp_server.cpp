//#include<iostream>
//#include<string>
//#include<winsock.h>
//#include<thread>
//#include <fstream> 
//
//#pragma comment(lib,"ws2_32.lib")
//using namespace std;
//
//
////数据包一共1500个字节
////0-4表示序列号，5-9表示确认序列号
////10-14是标志位，分别保留位、A（ack）、R（reset）、S（syn）、F（fin）
////15-19是校验和，20-1499是数据
//char message[1500];
//char recvBuf[1500];
//
//
////套接字初始化
//void inits()
//{
//	WORD sockVersion = MAKEWORD(2, 2);
//	WSADATA wsaData;
//	if (WSAStartup(sockVersion, &wsaData) != 0)
//	{
//		cout << "没有正确初始化socket" << endl;
//		return;
//	}
//}
//
//
//void initc()
//{
//	for (int i = 0; i < 1500; i++)
//	{
//		message[i] = '0';
//	}
//	message[1499] = '\0';
//}
//
//
//int char_to_num(int start, int end, char* ch)
//{
//	string str;
//	for (int i = start; i <= end; i++)
//	{
//		str += ch[i];
//	}
//	int n = atoi(str.c_str());
//	return n;
//}
//
//
//void num_to_char(int start, int end, int n)
//{
//	string str = to_string(n);
//	int j = str.length();
//	for (int i = end; i >= start && j >= 1; i--)
//	{
//		message[i] = str[j - 1];
//		j--;
//	}
//}
//
//
//int check(char* ch)
//{
//	int sum = 0;
//	for (int i = 0; i < 1500; i++)
//	{
//		if (i >= 15 && i <= 19) { continue; }
//		sum += abs((int)ch[i]) % 10;
//	}
//	return sum;
//}
//
//
//int print()
//{
//	if (message[11] == '1')
//	{
//		cout << "ack";
//	}
//	if (message[13] == '1')
//	{
//		cout << " " << "syn" << endl;
//	}
//	else if (recvBuf[11] == '1' && recvBuf[14] == '1')
//	{
//		cout << " " << "fin" << endl;
//		return 1;
//	}
//	else
//	{
//		cout << endl;
//	}
//	return 0;
//}
//
//
//void mywrite()
//{
//	if (recvBuf[13] != '1' && recvBuf[14] != '1' && recvBuf[10] != '1')
//	{
//		ofstream ofile("D:\\4.jpg", ios::app | ios::binary | ios::out);
//		ofile.write(recvBuf + 20, 1480);
//	}
//	else if (recvBuf[13] != '1' && recvBuf[14] != '1' && recvBuf[10] == '1')
//	{
//		ofstream ofile("D:\\4.jpg", ios::app | ios::binary | ios::out);
//		ofile.write(recvBuf + 20, char_to_num(1495, 1499, recvBuf));
//	}
//}
//
//
//void mysend(SOCKET sockServer, SOCKADDR_IN addrClient, int nAddrlen)
//{
//	if (recvBuf[13] == '1')
//	{
//		num_to_char(13, 13, 1);
//		num_to_char(11, 11, 1);
//	}
//	if (recvBuf[11] == '1' && recvBuf[14] == '1')
//	{
//		num_to_char(11, 11, 1);
//		num_to_char(14, 14, 1);
//	}
//
//	num_to_char(0, 4, 1);
//	num_to_char(15, 19, check(message));
//	num_to_char(11, 11, 1);
//
//	sendto(sockServer, message, 1500, 0, (SOCKADDR*)&addrClient, nAddrlen);
//}
//
//
//int main()
//{
//	inits();
//	initc();
//
//	SOCKET sockServer = socket(AF_INET, SOCK_DGRAM, 0);
//	SOCKADDR_IN addrServer;
//	addrServer.sin_family = AF_INET;
//	addrServer.sin_port = htons(1234);
//	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
//	if (bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)) == SOCKET_ERROR)
//	{
//		cout << "端口绑定失败！" << endl;
//		return -1;
//	}
//
//	SOCKADDR_IN addrClient;
//	int nAddrlen = sizeof(addrClient);
//
//	int exp_seq = 0;
//
//
//	cout << "receiving" << endl;
//	for (int i = 0;; i++)
//	{
//		int ret = recvfrom(sockServer, recvBuf, 1500, 0, (SOCKADDR*)&addrClient, &nAddrlen);
//		if (ret == 0)
//		{
//			cout << "没有收到" << endl;
//			return -1;
//		}
//		else
//		{
//			recvBuf[1500] = '\0';
//
//			int seq = char_to_num(0, 4, recvBuf);
//			int exp = char_to_num(5, 9, recvBuf);
//			int che = char_to_num(15, 19, recvBuf);
//			initc();
//			if (che != check(recvBuf))
//			{
//				cout << "校验和不对，请重传" << endl;
//				num_to_char(5, 9, seq);
//			}
//			else
//			{
//				if (recvBuf[13] == '1')
//				{
//					exp_seq = seq + 1;
//				}
//				else
//				{
//					if (seq == exp_seq)
//					{
//						exp_seq++;
//					}
//				}
//				num_to_char(5, 9, exp_seq);
//				cout << "已接收" << seq << "号数据包" << endl;
//				mywrite();
//			}
//			//if (i <= 10)
//			//{
//			//	mysend(sockServer, addrClient, nAddrlen);
//			//}
//
//			mysend(sockServer, addrClient, nAddrlen);
//
//			int ret = print();
//			if (ret == 1)
//			{
//				break;
//			}
//			
//		}
//	}
//	closesocket(sockServer);
//	WSACleanup();
//	return(0);
//}




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
char message[1500];
char recvBuf[1500];


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
	for (int i = 0; i < 1500; i++)
	{
		message[i] = '0';
	}
	message[1499] = '\0';
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
	for (int i = 0; i < strlen(ch); i++)
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
	if (recvBuf[13] != '1' && recvBuf[14] != '1' && recvBuf[10] != '1')
	{
		ofstream ofile(path, ios::app | ios::binary | ios::out);
		ofile.write(recvBuf + 20, 1480);
	}
	else if (recvBuf[13] != '1' && recvBuf[14] != '1' && recvBuf[10] == '1')
	{
		ofstream ofile(path, ios::app | ios::binary | ios::out);
		ofile.write(recvBuf + 20, char_to_num(1495, 1499, recvBuf));
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

	num_to_char(0, 4, 1);
	num_to_char(15, 19, check(message));
	num_to_char(11, 11, 1);

	sendto(sockServer, message, 1500, 0, (SOCKADDR*)&addrClient, nAddrlen);
}


int main()
{
	inits();
	initc();

	SOCKET sockServer = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(1234);
	addrServer.sin_addr.S_un.S_addr = inet_addr("10.136.136.135");
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
		int ret = recvfrom(sockServer, recvBuf, 1500, 0, (SOCKADDR*)&addrClient, &nAddrlen);
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
					exp_seq = seq + 1;
				}
				else
				{
					if (seq == exp_seq)
					{
						exp_seq++;
					}
				}
				num_to_char(5, 9, exp_seq);
				cout << "已接收" << seq << "号数据包" << endl;
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
			if (i <= 10)
			{
				mysend(sockServer, addrClient, nAddrlen);
			}

			//mysend(sockServer, addrClient, nAddrlen);

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
