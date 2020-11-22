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

//数据包一共1500个字节，0-4表示序列号，5-9表示确认序列号，10-14是校验和，15-1499是数据
char message[1500];


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


int read(int offset)
{
    ifstream infile;
    infile.open("D:\\helloworld.txt", ios::in | ios::binary);
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    if (offset > length) { return 1; }
    infile.seekg(offset);
    if (!infile.is_open()) return -1;
    char buf[1486];
    infile.read(buf, 1485);
    buf[1485] = '\0';

    for (int i = 0; i < 1485; i++)
    {
        message[15 + i] = buf[i];
    }
    return 0;
}


void num_to_char(int start, int end, int n)
{
    string str = to_string(n);
    int j = 0;
    for (int i = 0; i < 5; i++)
    {
        if (i < 5 - str.length())
            message[start+i] = '0';
        else
        {
            message[start+i] = str[j];
            j++;
        }
    }
}

int check(char* ch)
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

    //read_pic();
    SOCKET sockClient= socket(AF_INET, SOCK_DGRAM, 0);

    SOCKADDR_IN addrServer;
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(1234);
    addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    int SerAddrlen = sizeof(addrServer);



    for (int i = 0;; i++)
    {
        memset(message, 0, sizeof(message));
        num_to_char(0, 4, i % 2);
        num_to_char(5, 9, 1);
        num_to_char(10, 14, 0);
        if (read(i * 1485) == 1) { break; }
        num_to_char(10, 14, check(message));


        sendto(sockClient, message, 1500, 0, (SOCKADDR*)&addrServer, SerAddrlen);
        cout << "已发送" << i % 2 << "号数据包" << endl;


        char recvBuf[1500];
        int ret = 0; int flag = 0;
        clock_t now = clock();
        while (1)
        {
            ret = recvfrom(sockClient, recvBuf, 1500, 0, (SOCKADDR*)&addrServer, &SerAddrlen);
            cout << clock() - now << endl;
            if (clock() - now == 1000)
            {
                i--;
                flag = 1;
                break;
            }
            break;
        }
        if (flag == 0)
        {
            if (ret == 0)
            {
                cout << "error" << endl;
                return -1;
            }
            else
            {
                //cout << "已接收" << endl;
                if (recvBuf[9] == i % 2)
                {
                    i--;
                    continue;
                }
                cout << recvBuf[15] << recvBuf[16] << recvBuf[17] << endl;
            }
        }
        else
        {
            flag = 0;
        }
    }

    closesocket(sockClient);
    WSACleanup();
    return 0;
}