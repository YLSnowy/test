#include<iostream>
#include<winsock.h>
#include<ctime>
#include<thread>

#pragma comment(lib,"ws2_32.lib")
using namespace std;

//自己定义的数据格式
struct message
{
    char str[4096];
};
message m;

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

//向服务器发送数据的线程
void thread_send(SOCKET s_server)
{
    char buf[4096];
    while (1)
    {
        char reply[4096]; cin >> reply; strcpy(m.str, reply);

        memcpy(buf, &m, 4096);
        int len = send(s_server, buf, 4096, 0);
        if (len <= 0)
        {
            cout << "发送信息失败" << endl;
            closesocket(s_server);
        }
    }
}

//从服务器接收数据的线程
void thread_recv(SOCKET s_server)
{
    while (1)
    {
        message m1;
        char buf[4096];
        memset(buf, 0, sizeof(buf));
        int len = recv(s_server, buf, 4096, 0);
        memcpy(&m1, buf, 4096);
        if (len > 0)
        {
            cout << m1.str << endl;
        }
    }
}

int main()
{
    init();

    SOCKET s_server = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(1234);
    server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


    if (connect(s_server, (SOCKADDR*)&server, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        cout << "没有正确连接到服务器，连接关闭" << endl;
        WSACleanup();
        return 0;
    }
    else
    {
        cout << "已成功连接到服务器，开始聊天吧" << endl;
    }

    thread t1(thread_recv, s_server);
    thread t2(thread_send, s_server);
    t1.join();
    t2.join();

    closesocket(s_server);
    WSACleanup();
    return 0;
}