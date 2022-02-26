#define _CRT_SECURE_NO_WARNINGS
#include "login.h"
Login::Login()
{
	info.user = "root";
	info.password = "123";
	info.host = "localhost";
	info.port = 3306;
	info.database = "mychat";
	info.unix_socket = NULL;
	info.clientflag = 0;
	m_sqlManager.Init(info);
	memset(m_query, 0, sizeof(m_query));
}
Login::~Login()
{
	m_sqlManager.FreeConnect();
}
// 在数据库中查找是否有该用户
bool Login::loginSuccess(char* userName, char* userPwd)
{
	//char* query = "select * from user where";
	//memset(query, 0, sizeof(query));
	sprintf(m_query, "select * from user where user_name='%s' and user_pwd='%s'", userName, userPwd);
	return m_sqlManager.hasData(m_query);
}
