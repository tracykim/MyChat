#pragma once
#include <winsock2.h>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <conio.h>
#include <winsock2.h> 
#include <ws2tcpip.h>//定义socklen_t
#define SERVER_PORT 8888// 服务器端口号
#define SERVER_IP "127.0.0.1"// 默认服务器端IP地址
using namespace std;
class client
{
public:
	client();
	void init();
	void process();
	void sendata();

private:
	int user; //客户端套接字（句柄）
	int writing;
	sockaddr_in  serverAddr;
};