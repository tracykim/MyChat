//Client
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "client.h"
#pragma comment(lib, "ws2_32.lib")  // Á´½Óµ½WS2_32.lib

using namespace std;

int main()
{
	client user;
	user.process();
	return 0;
}

