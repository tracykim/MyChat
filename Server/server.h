#pragma once
#include <winsock2.h> 
#include <stdio.h>
#include <vector>
#include <ws2tcpip.h>//����socklen_t
#include <vector>

#define SERVER_IP "127.0.0.1"// Ĭ�Ϸ�������IP��ַ
#define SERVER_PORT 8888// �������˿ں�
class server
{
public:
	server();
	void init();
	void process();
	void processSingleChat();
private:
	int listener;//�����׽���
	sockaddr_in  serverAddr;//IPV4�ĵ�ַ��ʽ
	std::vector <int> socksArr;//��Ŵ������׽���
};