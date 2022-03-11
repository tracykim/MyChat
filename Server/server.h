#pragma once
//#include <Winsock2.h> 
#include "login.h"
#include "server_config.h"
#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <ws2tcpip.h>//定义socklen_t

void getUserAndMessage(const char buf[1024], char userName[128], char message[128]);

struct ClientInfo 
{
	ClientInfo(int clientArr_, std::string clientName_, bool recvLogin_)
	{
		clientArr = clientArr_;
		clientName = clientName_;
		recvLogin = recvLogin_;
	}
	int clientArr; // 客户端套接字
	std::string clientName; // 客户端用户名
	bool recvLogin; // 客户端是否登录
};
class server
{
public:
	server();
	void init();
	void process();
private:
	int listener;//监听套接字
	sockaddr_in  serverAddr;//IPV4的地址方式
	//std::vector <int> socksArr;//存放创建的套接字
	//std::vector <std::string> m_userName;//套接字对应的用户名
	//std::vector<bool> m_recvLogins;
	std::vector<ClientInfo> m_clientInfo;
	std::unordered_map<std::string, int> m_name_arr; //用户名-套接字

	Login m_db;
	std::vector<ClientInfo> m_groupList; // 群聊列表
	//void processMessage(const char* buf, int clinetIdx);
	void processData(char& dataType, char fromName[128], char toName[128], char data[1024], const char* buf);

	//bool sendUser(char message[1024], std::string fromClinetName, std::string toClinetName);
	//bool sendUser(std::string message, std::string toClinetName);
	bool sendUser(char message[1024], std::string toClinetName, int messageLen);

	//bool sendUserByGroup(char message[1024], std::string toClinetName);
	bool sendGroupMessage(char message[1024], int messageLen, char groupName[128], std::string fromClinetName);

};