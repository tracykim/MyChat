//Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "client.h"
#include "MySQLManager.h"
#pragma comment(lib, "ws2_32.lib")  // ���ӵ�WS2_32.lib

using namespace std;

int main()
{
	// ��¼�ͻ���
	client user;
	user.process();
	return 0;
}

