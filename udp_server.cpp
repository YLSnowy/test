#include<iostream>
#include<string>
#include<winsock.h>
#include<thread>
#include<list>
#pragma comment(lib,"ws2_32.lib")
using namespace std;



//自己定义的数据格式
struct message
{
	char str[4096];      //真正的交流数据
};
message m;

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

//向客户端发送消息的进程
void thread_send(SOCKET s_client)
{
	cout << "开始聊天吧！" << endl;
	char buf[4096];
	while (1)
	{
		char reply[4096]; cin >> reply; strcpy(m.str, reply);

		memcpy(buf, &m, 4096);
		send(s_client, buf, 4096, 0);
	}
}

//从客户端接收消息的进程
void thread_recv(SOCKET s_client)
{
	while (1)
	{
		message m1;
		char buf[4096];
		memset(buf, 0, sizeof(buf));
		int ret = recv(s_client, buf, 4096, 0);
		memcpy(&m1, buf, 4096);
		if (ret >= 0)
		{
			cout << m1.str << endl;
		}
	}
}


int main()
{
	init();

	SOCKET s_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SOCKADDR_IN ListenAddr;
	ListenAddr.sin_family = AF_INET;
	ListenAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	ListenAddr.sin_port = htons(1234);

	if (bind(s_server, (LPSOCKADDR)&ListenAddr, sizeof(ListenAddr)) == SOCKET_ERROR)
	{
		cout << "端口绑定失败！" << endl;
		return -1;
	}

	listen(s_server, 5);
	cout << "服务端准备就绪，等待连接请求" << endl;


	while (1)
	{
		SOCKET* s_client = new SOCKET;
		s_client = (SOCKET*)malloc(sizeof(SOCKET));
		int SockAddrlen = sizeof(sockaddr);
		*s_client = accept(s_server, 0, 0);
		cout << "一个客户端已连接到服务器，socket是：" << *s_client << endl;



		thread t1(thread_send, *s_client);
		thread t2(thread_recv, *s_client);
		t1.join();
		t2.join();
		//CreateThread(NULL, 0, &ServerThread, s_client, 0, NULL);
	}
	closesocket(s_server);
	WSACleanup();
	return(0);
}