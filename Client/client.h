#pragma once
#include <winsock2.h>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <conio.h>
#include <winsock2.h> 
#include <ws2tcpip.h>//����socklen_t
#include "MySQLManager.h"
#include "login.h"
#include "user.h"
#define SERVER_PORT 8888// �������˿ں�
#define SERVER_IP "127.0.0.1"// Ĭ�Ϸ�������IP��ַ
using namespace std;
class client
{
public:
	client();
	void init();
	void process();
	void sendata();
	void printHelp();
private:
	int m_handle; //�ͻ����׽��֣������
	int writing;
	sockaddr_in  serverAddr;
	Login m_login;
	User m_user;
};