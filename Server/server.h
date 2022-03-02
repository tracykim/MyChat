#pragma once
#include <winsock2.h> 
#include <stdio.h>
#include <vector>
#include <ws2tcpip.h>//定义socklen_t
#include <vector>

#define SERVER_IP "127.0.0.1"// 默认服务器端IP地址
#define SERVER_PORT 8888// 服务器端口号
class server
{
public:
	server();
	void init();
	void process();
	void processSingleChat();
private:
	int listener;//监听套接字
	sockaddr_in  serverAddr;//IPV4的地址方式
	std::vector <int> socksArr;//存放创建的套接字
};