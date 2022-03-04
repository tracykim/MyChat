#pragma once
#include <winsock2.h> 
#include <stdio.h>
#include <vector>
#include <ws2tcpip.h>//����socklen_t
#include <vector>
#include <unordered_map>
#include "server_config.h"

void getUserAndMessage(const char buf[1024], char userName[128], char message[128]);

struct ClientInfo 
{
	ClientInfo(int clientArr_, std::string clientName_, bool recvLogin_)
	{
		clientArr = clientArr_;
		clientName = clientName_;
		recvLogin = recvLogin_;
	}
	int clientArr; // �ͻ����׽���
	std::string clientName; // �ͻ����û���
	bool recvLogin; // �ͻ����Ƿ��¼
};
class server
{
public:
	server();
	void init();
	void processSingleChat();
private:
	int listener;//�����׽���
	sockaddr_in  serverAddr;//IPV4�ĵ�ַ��ʽ
	//std::vector <int> socksArr;//��Ŵ������׽���
	//std::vector <std::string> m_userName;//�׽��ֶ�Ӧ���û���
	//std::vector<bool> m_recvLogins;
	std::vector<ClientInfo> m_clientInfo;
	std::unordered_map<std::string, int> m_name_arr; //�û���-�������׽���
};