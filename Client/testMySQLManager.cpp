#include "client.h"
#include "MySQLManager.h"
#include "login.h"
#pragma comment(lib, "ws2_32.lib")  // 链接到WS2_32.lib

using namespace std;
/*
int main() {
	/// 1、测试mysql连接 增删改查	
	//MySQLManager mysql;
	//ConnectionInfo info;
	//// 填充ConnectionInfo这个结构体，项目中一般从配置文件这读取
	//info.user = "root";
	//info.password = "123";
	//info.host = "localhost";
	//info.port = 3306;
	//info.database = "mychat";
	//info.unix_socket = NULL;
	//info.clientflag = 0;

	//// mysql连接
	//if (!mysql.Init(info))
	//{
	//	return -1;
	//}

	//// 增加数据测试
	//const char* sql1 = "insert into user values (NULL, 'Kim', 'qwe')";
	//mysql.ExecuteSql(sql1);

	//// 删除数据测试
	////const char* sql2 = "delete from user where name = 'AceTan'";
	////mysql.ExecuteSql(sql2);

	//// 修改数据测试
	////const char* sql3 = "update user set password='update_password' where name = 'Ada'";
	////mysql.ExecuteSql(sql3);

	//// 查询数据测试
	//const char* sql4 = "select * from user";
	//mysql.QueryData(sql4);
	//mysql.PrintQueryRes();

	//// 释放mysql资源
	//mysql.FreeConnect();

	/// 2、测试登录功能
	//char userName[] = "jinye";
	//char userPwd[] = "123";
	//Login login;
	//if (login.loginSuccess(userName, userPwd))
	//	cout << "login success" << endl;
	//else
	//	cout << "login failed" << endl;
}*/