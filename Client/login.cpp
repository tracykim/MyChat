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
// �����ݿ��в����Ƿ��и��û�
bool Login::loginSuccess(char* userName, char* userPwd)
{
	//char* query = "select * from user where";
	//memset(query, 0, sizeof(query));
	sprintf(m_query, "SELECT * FROM user WHERE user_name='%s' and user_pwd='%s'", userName, userPwd);
	if (m_sqlManager.hasData(m_query)) // �û�����������ȷ����½�ɹ�
	{
		return true;
	}
	else
	{
		sprintf(m_query, "select * from user where user_name='%s'", userName);
		if(m_sqlManager.hasData(m_query)){
			printf("�û����������벻��ȷ\n");
			return false;
		}
		else {// ע�����û�
			sprintf(m_query, "INSERT INTO user VALUES (NULL, '%s', '%s')", userName, userPwd);
			m_sqlManager.ExecuteSql(m_query);
			return true;
		}
	}
	
}

void Login::QueryFriendList(char* userName)
{
	sprintf(m_query, "SELECT user_name2 AS user_name FROM friend_list WHERE user_name1='%s' UNION ALL SELECT user_name1 FROM friend_list WHERE user_name2 ='%s'", userName, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.PrintQueryRes();
}

void Login::QueryGroupList(char * userName)
{
}
