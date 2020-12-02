//#include <fstream>
//#include <iostream>
//#include <winsock.h>
//#include <ctime>
//#include <time.h>
//#include <thread>
//#include <string>
//#include <math.h>
//#pragma comment(lib,"ws2_32.lib")
//using namespace std;
//
////int flag = 0;
//
////数据包一共1500个字节
////0-4表示序列号，5-9表示确认序列号
////10-14是标志位，分别保留位、A（ack）、R（reset）、S（syn）、F（fin），保留位在判断是否是最后一个包的时候用到了
////15-19是校验和，20-1499是数据
//char sendBuf[10][1500];
//char message[1500];
//char recvBuf[1500];
//
//
//void inits()
//{
//    WORD sockVersion = MAKEWORD(2, 2);
//    WSADATA wsaData;
//    if (WSAStartup(sockVersion, &wsaData) != 0)
//    {
//        cout << "没有正确初始化socket" << endl;
//        return;
//    }
//}
//
//
//void initc()
//{
//    for (int i = 0; i < 1500; i++)
//    {
//        message[i] = '0';
//    }
//    message[1499] = '\0';
//    for (int i = 0; i < 10; i++)
//    {
//        for (int j = 0; j < 1500; j++)
//        {
//            sendBuf[i][j] = '0';
//        }
//        sendBuf[i][1499] = '\0';
//    }
//}
//
//
//void num_to_char(int start, int end, int n, char* ch)
//{
//    string str = to_string(n);
//    int j = str.length();
//    for (int i = end; i >= start && j >= 1; i--)
//    {
//        ch[i] = str[j - 1];
//        j--;
//    }
//}
//
//
//int char_to_num(int start, int end, char* ch)
//{
//    string str;
//    for (int i = start; i <= end; i++)
//    {
//        str += ch[i];
//    }
//    int n = atoi(str.c_str());
//    return n;
//}
//
//
//int read(int offset, char* ch,char* path)
//{
//    ifstream infile;
//    infile.open(path, ios::in | ios::binary);
//    if (!infile.is_open()) return -1;
//
//    infile.seekg(0, infile.end);
//    int length = infile.tellg();
//    if (offset > length) { return 1; }
//
//    infile.seekg(offset);
//
//    if (length - offset < 1480)
//    {
//        char* buf = new char[length - offset + 1];
//        infile.read(buf, length - offset);
//        buf[length - offset] = '\0';
//        num_to_char(10, 10, 1, ch);
//        for (int i = 0; i < length - offset; i++)
//        {
//            ch[20 + i] = buf[i];
//        }
//        num_to_char(1495, 1499, length - offset, ch);
//        return 2;
//    }
//    else
//    {
//        char buf[1481];
//        infile.read(buf, 1480);
//        buf[1480] = '\0';
//
//        for (int i = 0; i < 1480; i++) 
//        {
//            ch[20 + i] = buf[i]; 
//        }
//    }
//    return 0;
//}
//
//
//int make_pkt(int base, int& end)
//{
//    initc();
//    int j = 0;
//    for (int i = base; i < end; i++)
//    {
//        int ret = read(i * 1480, sendBuf[j]);
//        j++;
//        if (ret == 2)
//        {
//            end = j;
//            return 1;
//        }
//    }
//    end = 10;
//    return 0;
//}
//
//
//int check(char* ch)
//{
//    int sum = 0;
//    for (int i = 0; i < 1500; i++)
//    {
//        if (i >= 15 && i <= 19) { continue; }
//        sum += abs((int)ch[i]) % 10;
//    }
//    return sum;
//}
//
//
//void myrecv(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen, int number,int& flag)
//{
//    int ret = recvfrom(sockClient, recvBuf, 1500, 0, (SOCKADDR*)&addrServer, &SerAddrlen);
//    if (ret < 0)
//    {
//        cout << "服务器端没有打开连接，请等待一段时间再试" << endl;
//        flag = -1;
//        return;
//    }
//    flag = 1;
//    return;
//}
//
//
//int shake(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen)
//{
//    num_to_char(0, 4, 0, message);
//    num_to_char(5, 9, 0, message);
//    num_to_char(13, 13, 1, message);
//    num_to_char(15, 19, check(message), message);
//    while (1)
//    {
//        sendto(sockClient, message, 1500, 0, (SOCKADDR*)&addrServer, SerAddrlen);
//        cout << "建立连接中" << endl;
//        int flag = 0, flag1 = 0;
//        //thread t1(myrecv, sockClient, addrServer, SerAddrlen, ref(flag));
//        //clock_t now = clock();
//        //while (1) {
//        //    if (flag == 1)
//        //    {
//        //        flag1 = 0;
//        //        break;
//        //    }
//        //    else if (flag == -1)
//        //    {
//        //        t1.detach();
//        //        return -1;
//        //    }
//        //    if ((double)(clock() - now) >= 3000)
//        //    {
//        //        flag1 = 1;
//        //        break;
//        //    }
//        //}
//        //t1.detach();
//
//        myrecv(sockClient, addrServer, SerAddrlen, 1, ref(flag));
//        if (flag1 == 0)
//        {
//            if (recvBuf[11] == '1' && recvBuf[13] == '1')
//            {
//                cout << "已成功建立连接" << endl;
//            }
//            break;
//        }
//    }
//
//    //myrecv(sockClient, addrServer, SerAddrlen);
//    return 0;
//}
//
//
//int transfer(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen, int base, int end)
//{
//    int seq = base;
//    int exp = 0;
//    for (int i = 0; i < end - base; i++)
//    {
//        num_to_char(0, 4, seq, sendBuf[i]);
//        num_to_char(5, 9, 1, sendBuf[i]);
//        //传输过程中标志位不需要赋值
//        num_to_char(15, 19, 0, sendBuf[i]);
//        num_to_char(15, 19, check(sendBuf[i]), sendBuf[i]);
//
//        sendto(sockClient, sendBuf[i], 1500, 0, (SOCKADDR*)&addrServer, SerAddrlen);
//        cout << "已发送" << seq << "号数据包" << endl;
//    }
//    return 0;
//}
//
//
//int timer(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen, int base, int end, int& flag1)
//{
//    for (int i = base; i < end; i++)
//    {
//        int flag = 0;
//        thread t1(myrecv, sockClient, addrServer, SerAddrlen, base, ref(flag));
//        clock_t now = clock();
//        while (1) {
//            if (flag == 1)
//            {
//                flag = 0;
//                flag1 = 0;
//                break;
//            }
//            else if (flag == -1)
//            {
//                flag = 0;
//                t1.detach();
//                return -1;
//            }
//            if ((double)(clock() - now) >= 3000)
//            {
//                flag1 = 1;
//                break;
//            }
//        }
//        t1.detach();
//    }
//}
//
//
//int close(SOCKET sockClient, SOCKADDR_IN addrServer, int SerAddrlen)
//{
//    while (1)
//    {
//        initc();
//        num_to_char(0, 4, 0, message);
//        num_to_char(5, 9, 0, message);
//        num_to_char(11, 11, 1, message);
//        num_to_char(14, 14, 1, message);
//        num_to_char(15, 19, check(message), message);
//        sendto(sockClient, message, 1500, 0, (SOCKADDR*)&addrServer, SerAddrlen);
//        cout << "已发送结束数据包" << endl;
//        int ret = recvfrom(sockClient, recvBuf, 1500, 0, (SOCKADDR*)&addrServer, &SerAddrlen);
//        if (ret < 0)
//        {
//            cout << "error" << endl;
//            return -1;
//        }
//        if (recvBuf[11] == '1' && recvBuf[14] == '1' && ret > 0)
//        {
//            closesocket(sockClient);
//            WSACleanup();
//            break;
//        }
//    }
//}
//
//
//int main()
//{
//    inits();
//    initc();
//
//    SOCKET sockClient = socket(AF_INET, SOCK_DGRAM, 0);
//
//    SOCKADDR_IN addrServer;
//    addrServer.sin_family = AF_INET;
//    addrServer.sin_port = htons(1234);
//    addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
//    int SerAddrlen = sizeof(addrServer);
//
//
//
//    int ret;
//    ret = shake(sockClient, addrServer, SerAddrlen);
//    if (ret == -1)
//    {
//        return 0;
//    }
//
//
//    int base = 0, end = 0;
//    while (1)
//    {
//        ret = make_pkt(base, end);
//        
//        transfer(sockClient, addrServer, SerAddrlen, base, end);
//
//        int flag = 0, flag1 = 0;
//        ret = timer(sockClient, addrServer, SerAddrlen, base, end, flag1);
//        
//        
//        if (ret == 1)
//        {
//            break;
//        }
//        base++;
//        end = base + 10;
//    }
//
//
//    ret = close(sockClient, addrServer, SerAddrlen);
//    if ( ret == -1)
//    {
//        return 0;
//    }
//    return 0;
//}




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
//10-14是标志位，分别保留位、A（ack）、T（type）、S（syn）、F（fin）
//保留位在判断是否是最后一个包的时候用到了，类型表示发送的是文字还是图片，便于接收方进行存储
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


int read(int offset,const char* path)
{
    ifstream infile;
    infile.open(path, ios::in | ios::binary);
    infile.seekg(0, infile.end);
    int length = infile.tellg();
    if (offset > length) { return 1; }
    infile.seekg(offset);
    if (!infile.is_open()) return -1;

    if (path[strlen(path) - 1] == 'g')
    {
        num_to_char(12, 12, 0);
    }
    else if (path[strlen(path) - 1] == 't')
    {
        num_to_char(12, 12, 1);
    }

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
    string path;
    cout << "请输入你想发送的文件名称：" ;
    cin >> path;
    path = "D:\\" + path;
    const char* p = path.c_str();
    for (int i = 0;; i++)
    {
        initc();
        num_to_char(0, 4, seq);
        num_to_char(5, 9, 1);
        //传输过程中标志位不需要赋值
        num_to_char(15, 19, 0);
        if (read(i * 1480,p) == 1) { break; }
        num_to_char(15, 19, check(message));

        sendto(sockClient, message, 1500, 0, (SOCKADDR*)&addrServer, SerAddrlen);
        cout << "已发送" << seq << "号数据包" << endl;

        int flag = 0;
        int flag1 = 0;
        thread t1(myrecv, sockClient, addrServer, SerAddrlen, ref(flag));
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
    string s;
    cout << "请输入希望发送的服务器的ip地址：";
    cin >> s;
    const char* addr = s.c_str();
    addrServer.sin_addr.S_un.S_addr = inet_addr(addr);
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
    if (ret == -1)
    {
        return 0;
    }
    return 0;
}
