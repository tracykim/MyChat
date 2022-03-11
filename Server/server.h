#pragma once
//#include <Winsock2.h> 
#include "login.h"
#include "server_config.h"
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <ws2tcpip.h>//����socklen_t

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
	void process();
private:
	int listener;//�����׽���
	sockaddr_in  serverAddr;//IPV4�ĵ�ַ��ʽ
	//std::vector <int> socksArr;//��Ŵ������׽���
	//std::vector <std::string> m_userName;//�׽��ֶ�Ӧ���û���
	//std::vector<bool> m_recvLogins;
	std::vector<ClientInfo> m_clientInfo;
	std::unordered_map<std::string, int> m_name_arr; //�û���-�׽���

	Login m_db;
	std::vector<ClientInfo> m_groupList; // Ⱥ���б�
	//void processMessage(const char* buf, int clinetIdx);
	void processData(char& dataType, char fromName[128], char toName[128], char data[1024], const char* buf);

	//bool sendUser(char message[1024], std::string fromClinetName, std::string toClinetName);
	//bool sendUser(std::string message, std::string toClinetName);
	bool sendUser(char message[1024], std::string toClinetName, int messageLen);

	//bool sendUserByGroup(char message[1024], std::string toClinetName);
	bool sendGroupMessage(char message[1024], int messageLen, char groupName[128], std::string fromClinetName);

};