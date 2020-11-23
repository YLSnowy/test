#include <fstream>
#include <iostream>
#include <winsock.h>
#include <ctime>
#include <time.h>
#include <thread>
#include <string>
#include <math.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

int flag = 0;
char recvBuf[1500];



//数据包一共1500个字节
//0-4表示序列号，5-9表示确认序列号
//10-14是标志位，分别保留位、A（ack）、R（reset）、S（syn）、F（fin）
//15-19是校验和，20-1499是数据
char message[1500];


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


int read(int offset)
{
    ifstream infile;
    infile.open("D:\\helloworld.txt", ios::in | ios::binary);
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    if (offset > length) { return 1; }
    infile.seekg(offset);
    if (!infile.is_open()) return -1;
    char buf[1481];
    infile.read(buf, 1480);
    buf[1480] = '\0';

    for (int i = 0; i < 1480; i++)
    {
        message[20 + i] = buf[i];
    }
    return 0;
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


//DWORD WINAPI ServerThread(LPVOID lpParameter)
//{
//    SOCKET* ClientSocket = (SOCKET*)lpParameter;
//    while (1)
//    {
//        int ret = recvfrom(*ClientSocket, recvBuf, 1500, 0, (SOCKADDR*)&addrServer, &SerAddrlen);
//    }
//    closesocket(*ClientSocket);
//    free(ClientSocket);
//    return 0;
//}



void connect()
{

}

int main()
{
    inits();
    initc();

    SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);

    SOCKADDR_IN addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(1234);
    addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    int SerAddrlen = sizeof(addrServer);



    //if (flag == 0)
    //{
    //    memset(message, 0, sizeof(message));
    //    num_to_char(0, 4, 0);
    //    num_to_char(5, 9, 0);
    //    num_to_char(10, 14, 0);
    //    num_to_char(15, 19, 1);
    //    message[15] = 's';
    //    message[16];

    //    
    //    flag = 1;
    //}


    for (int i = 0;; i++)
    {
        initc();
        num_to_char(0, 4, i % 2);
        num_to_char(5, 9, 1);
        //传输过程中标志位不需要赋值
        num_to_char(15, 19, 0);
        if (read(i * 1480) == 1) { break; }
        num_to_char(15, 19, check(message));

        sendto(sockClient, message, 1500, 0, (SOCKADDR*)&addrServer, SerAddrlen);
        cout << "已发送" << i % 2 << "号数据包" << endl;


        //int ret = 0; int flag = 0;
        //CreateThread(NULL, 0, &ServerThread, &sockClient, 0, NULL);
        int ret = recvfrom(sockClient, recvBuf, 1500, 0, (SOCKADDR*)&addrServer, &SerAddrlen);

        if (ret < 0)
        {
            cout << "error" << endl;
            return -1;
        }
        else
        {
            //cout << "已接收" << endl;
            if (recvBuf[11] == '0' || recvBuf[9] == i % 2+48)
            {
                i--;
                continue;
            }
            if (recvBuf[11] == '1' && recvBuf[9] != i % 2 + 48)
            {
                cout << "ack" << endl;
            }
        }
    }
    closesocket(sockClient);
    WSACleanup();
    return 0;
}