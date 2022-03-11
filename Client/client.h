#pragma once
//#include <winsock2.h>
#include <stdio.h>
#include <thread>
#include <iostream>
#include <conio.h>
#include <ws2tcpip.h>//定义socklen_t
#include "MySQLManager.h"
#include "useMySQL.h"
#include "user.h"
#include "server_config.h"
#include "dataProcess.h"
using namespace std;
class client
{
public:
	client();
	void init();
	void process();
	void sendata();
	void printHelp();
	bool messageIsVaild();
	void printFrinedList();
	void printGroupList();
private:
	int m_handle; //客户端套接字（句柄）
	int writing;
	sockaddr_in  serverAddr;
	bool m_sendLogin = false;
	UseDB m_db;
	User m_user;


	bool m_quit = false;
	std::vector<string> m_friendList;
	std::vector<string> m_groupList;

	//void splitData(int dataType, const char* fromName, const char* toName, const char* data, char* sendbuf);
	//void processData(char& dataType, char fromName[128], char toName[128], char data[1024], const char* buf);
};