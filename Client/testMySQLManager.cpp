#include "client.h"
#include "MySQLManager.h"
#include "login.h"
#pragma comment(lib, "ws2_32.lib")  // ���ӵ�WS2_32.lib

using namespace std;
/*
int main() {
	/// 1������mysql���� ��ɾ�Ĳ�	
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

	//// mysql����
	//if (!mysql.Init(info))
	//{
	//	return -1;
	//}

	//// �������ݲ���
	//const char* sql1 = "insert into user values (NULL, 'Kim', 'qwe')";
	//mysql.ExecuteSql(sql1);

	//// ɾ�����ݲ���
	////const char* sql2 = "delete from user where name = 'AceTan'";
	////mysql.ExecuteSql(sql2);

	//// �޸����ݲ���
	////const char* sql3 = "update user set password='update_password' where name = 'Ada'";
	////mysql.ExecuteSql(sql3);

	//// ��ѯ���ݲ���
	//const char* sql4 = "select * from user";
	//mysql.QueryData(sql4);
	//mysql.PrintQueryRes();

	//// �ͷ�mysql��Դ
	//mysql.FreeConnect();

	/// 2�����Ե�¼����
	//char userName[] = "jinye";
	//char userPwd[] = "123";
	//Login login;
	//if (login.loginSuccess(userName, userPwd))
	//	cout << "login success" << endl;
	//else
	//	cout << "login failed" << endl;
}*/