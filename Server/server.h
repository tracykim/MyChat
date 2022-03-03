#pragma once
#include <winsock2.h> 
#include <stdio.h>
#include <vector>
#include <ws2tcpip.h>//定义socklen_t
#include <vector>
#include <unordered_map>
#include "server_config.h"

void getUserAndMessage(const char buf[1024], char userName[128], char message[128]);

class server
{
public:
	server();
	void init();
	void processSingleChat();
private:
	int listener;//监听套接字
	sockaddr_in  serverAddr;//IPV4的地址方式
	std::vector <int> socksArr;//存放创建的套接字
	std::unordered_map<std::string, int> m_clientInfo; //用户名-创建的套接字
	std::vector<bool> m_recvLogins;
};