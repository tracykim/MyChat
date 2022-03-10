//server
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "server.h"
//#include "MySQLManager.h"
#pragma comment(lib,"ws2_32.lib")// Á´½Óµ½WS2_32.lib
#pragma comment(lib, "libmysql.lib")

using namespace std;
int main()
{
	server ser;
	ser.process();
	return 0;
}


