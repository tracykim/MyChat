#pragma once
#include <winsock2.h> 
#include <stdio.h>
#include <vector>
#include <ws2tcpip.h>//����socklen_t
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
	int listener;//�����׽���
	sockaddr_in  serverAddr;//IPV4�ĵ�ַ��ʽ
	std::vector <int> socksArr;//��Ŵ������׽���
	std::unordered_map<std::string, int> m_clientInfo; //�û���-�������׽���
	std::vector<bool> m_recvLogins;
};