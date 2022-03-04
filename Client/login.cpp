#define _CRT_SECURE_NO_WARNINGS
#include "login.h"
Login::Login()
{
	info.user = DB_USERNAME;
	info.password = DB_PASSWORD;
	info.host = HOST; // "localhost";
	info.port = PORT;
	info.database = DATABASE_NAME;
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
	sprintf(m_query, "SELECT * FROM %s WHERE user_name='%s' and user_pwd='%s'", TABLE_USER, userName, userPwd);
	if (m_sqlManager.hasData(m_query)) // 用户名、密码正确：登陆成功
	{
		return true;
	}
	else
	{
		sprintf(m_query, "select * from %s where user_name='%s'", TABLE_USER, userName);
		if(m_sqlManager.hasData(m_query)){
			printf("用户名或者密码不正确\n");
			return false;
		}
		else {// 注册新用户
			sprintf(m_query, "INSERT INTO %s VALUES (NULL, '%s', '%s')", TABLE_USER, userName, userPwd);
			m_sqlManager.ExecuteSql(m_query);
			return true;
		}
	}
	
}

void Login::QueryFriendList(char* userName)
{
	sprintf(m_query, "SELECT user_name2 AS user_name FROM %s WHERE user_name1='%s' UNION ALL SELECT user_name1 FROM %s WHERE user_name2 ='%s'", TABLE_FRIEND, userName, TABLE_FRIEND, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.PrintQueryRes();
}

void Login::QueryGroupList(char* userName)
{
	sprintf(m_query, "SELECT group_name FROM %s WHERE user_name='%s'", TABLE_GROUP, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.PrintQueryRes();
}

void Login::QueryChatList(char* userName)
{
	printf("好友列表：");
	QueryFriendList(userName);
	printf("群聊列表：");
	QueryGroupList(userName);
}

