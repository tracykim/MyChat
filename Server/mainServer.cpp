//server
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "server.h"
#include "MySQLManager.h"
#pragma comment(lib,"ws2_32.lib")// ���ӵ�WS2_32.lib
#pragma comment(lib, "libmysql.lib")

using namespace std;
int main()
{
	//MySQLManager mysql;
	//ConnectionInfo info;
	//// ���ConnectionInfo����ṹ�壬��Ŀ��һ��������ļ����ȡ
	//info.user = "root";
	//info.password = "123";
	//info.host = "localhost";
	//info.port = 3306;
	//info.database = "mychat";
	//info.unix_socket = NULL;
	//info.clientflag = 0;

	//const char* sql4 = "select * from user";
	//MYSQL_RES* res = mysql.QueryData(sql4);
	//mysql.PrintQueryRes();
	server ser;
	//ser.process();
	ser.processSingleChat();

	return 0;
}


