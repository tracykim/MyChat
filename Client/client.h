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
#include "server_config.h"
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
	bool m_sendLogin = false;
	User m_user;
};