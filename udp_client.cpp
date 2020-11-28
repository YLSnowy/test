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

//int flag = 0;

//数据包一共1500个字节
//0-4表示序列号，5-9表示确认序列号
//10-14是标志位，分别保留位、A（ack）、R（reset）、S（syn）、F（fin），保留位在判断是否是最后一个包的时候用到了
//15-19是校验和，20-1499是数据
char message[1500];
char recvBuf[1500];


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


int read(int offset)
{
    ifstream infile;
    infile.open("D:\\1.jpg", ios::in | ios::binary);
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    if (offset > length) { return 1; }
    infile.seekg(offset);
    if (!infile.is_open()) return -1;

    if (length - offset < 1480)
    {
        char* buf = new char[length - offset + 1];
        infile.read(buf, length - offset);
        buf[length - offset] = '\0';
        num_to_char(10, 10, 1);
        for (int i = 0; i < length - offset; i++)
        {
            message[20 + i] = buf[i];
        }
        num_to_char(1495, 1499, length - offset);
    }
    else
    {
        char buf[1481];
        infile.read(buf, 1480);
        buf[1480] = '\0';

        for (int i = 0; i < 1480; i++)
        {
            message[20 + i] = buf[i];
        }
    }
    return 0;
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


void myrecv(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen, int& flag)
{
    int ret = recvfrom(sockClient, recvBuf, 1500, 0, (SOCKADDR*)&addrServer, &SerAddrlen);
    if (ret < 0)
    {
        cout << "服务器端没有打开连接，请等待一段时间再试" << endl;
        flag = -1;
        return;
    }
    flag = 1;
    return;
}


int shake(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen)
{
    num_to_char(0, 4, 0);
    num_to_char(5, 9, 0);
    num_to_char(13, 13, 1);
    num_to_char(15, 19, check(message));
    while (1)
    {
        sendto(sockClient, message, 1500, 0, (SOCKADDR*)&addrServer, SerAddrlen);
        cout << "建立连接中" << endl;
        int flag = 0, flag1 = 0;
        thread t1(myrecv, sockClient, addrServer, SerAddrlen, ref(flag));
        clock_t now = clock();
        while (1) {
            if (flag == 1)
            {
                flag1 = 0;
                break;
            }
            else if (flag == -1)
            {
                t1.detach();
                return -1;
            }
            if ((double)(clock() - now) >= 3000)
            {
                flag1 = 1;
                break;
            }
        }
        t1.detach();
        //myrecv(sockClient, addrServer, SerAddrlen, ref(flag));
        if (flag1 == 0)
        {
            if (recvBuf[11] == '1' && recvBuf[13] == '1')
            {
                cout << "已成功建立连接" << endl;
            }
            break;
        }
    }

    //myrecv(sockClient, addrServer, SerAddrlen);
    return 0;
}


int transfer(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen)
{
    int seq = 1;
    int exp = 0;
    for (int i = 0;; i++)
    {
        initc();
        num_to_char(0, 4, seq);
        num_to_char(5, 9, 1);
        //传输过程中标志位不需要赋值
        num_to_char(15, 19, 0);
        if (read(i * 1480) == 1) { break; }
        num_to_char(15, 19, check(message));

        sendto(sockClient, message, 1500, 0, (SOCKADDR*)&addrServer, SerAddrlen);
        cout << "已发送" << seq << "号数据包" << endl;

        int flag = 0;
        int flag1 = 0;
        thread t1(myrecv, sockClient, addrServer, SerAddrlen,ref(flag));
        clock_t now = clock();
        while (1) {
            if (flag == 1)
            {
                flag1 = 0;
                //t1.detach();
                break;
            }
            else if (flag == -1)
            {
                t1.detach();
                return -1;
            }
            if ((double)(clock() - now) >= 3000)
            {
                flag1 = 1;
                //t1.detach();
                break;
            }
        }

        t1.detach();
        //myrecv(sockClient, addrServer, SerAddrlen, ref(flag));
        if (flag1 == 0)
        {
            exp = char_to_num(5, 9, recvBuf);
            seq = char_to_num(0, 4, message);
            if (exp == seq + 1)
            {
                cout << "ack" << seq << endl;
                seq = exp;
            }
            else 
            {
                cout << "失序重发" << seq << endl;
                i--;
                continue;
            }
        }
        else
        {
            i--;
            cout << "超时重发" << endl;
            continue;
        }
    }
    return 0;
}


int close(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen)
{
    while (1)
    {
        initc();
        num_to_char(0, 4, 0);
        num_to_char(5, 9, 0);
        num_to_char(11, 11, 1);
        num_to_char(14, 14, 1);
        num_to_char(15, 19, check(message));
        sendto(sockClient, message, 1500, 0, (SOCKADDR*)&addrServer, SerAddrlen);
        cout << "已发送结束数据包" << endl;
        int ret = recvfrom(sockClient, recvBuf, 1500, 0, (SOCKADDR*)&addrServer, &SerAddrlen);
        if (ret < 0)
        {
            cout << "error" << endl;
            return -1;
        }
        if (recvBuf[11] == '1' && recvBuf[14] == '1' && ret > 0)
        {
            closesocket(sockClient);
            WSACleanup();
            break;
        }
    }
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



    int ret;
    ret = shake(sockClient, addrServer, SerAddrlen);
    if (ret == -1)
    {
        return 0;
    }

    ret = transfer(sockClient, addrServer, SerAddrlen);
    if (ret == -1)
    {
        return 0;
    }

    ret = close(sockClient, addrServer, SerAddrlen);
    if ( ret == -1)
    {
        return 0;
    }
    return 0;
}