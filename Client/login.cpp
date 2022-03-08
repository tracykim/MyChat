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
	sprintf(m_query, "SELECT 1 FROM %s WHERE user_name='%s' and user_pwd='%s' limit 1", TABLE_USER, userName, userPwd);
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

// 查询好友列表
void Login::QueryFriendList(char* userName)
{
	sprintf(m_query, "SELECT user_name2 FROM %s WHERE user_name1='%s' UNION ALL SELECT user_name1 FROM %s WHERE user_name2 ='%s'", TABLE_FRIEND, userName, TABLE_FRIEND, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.PrintQueryRes();
}

// 查询好友列表并存储
void Login::QueryFriendList(char* userName, std::vector<string>& friendList)
{
	sprintf(m_query, "SELECT user_name2 FROM %s WHERE user_name1='%s' UNION ALL SELECT user_name1 FROM %s WHERE user_name2 ='%s'", TABLE_FRIEND, userName, TABLE_FRIEND, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.SaveQueryRes(friendList);
}

// 查询群聊列表
void Login::QueryGroupList(char* userName)
{
	sprintf(m_query, "SELECT group_name FROM %s WHERE user_name='%s'", TABLE_GROUP, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.PrintQueryRes();
}

// 查询群聊列表并存储
void Login::QueryGroupList(char* userName, std::vector<string>& groupList)
{
	sprintf(m_query, "SELECT group_name FROM %s WHERE user_name='%s'", TABLE_GROUP, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.SaveQueryRes(groupList);
}
void Login::QueryChatList(char* userName)
{
	printf("好友列表：");
	QueryFriendList(userName);
	printf("群聊列表：");
	QueryGroupList(userName);
}

// 添加好友
bool Login::addFriend(char * fromUserName, char * toUserName)
{
	// 查询该好友是否存在
	if (!hasUser(toUserName))
		return false;

	sprintf(m_query, "SELECT 1 FROM %s WHERE (user_name1='%s' AND user_name2='%s') OR (user_name2='%s' AND user_name1='%s')", TABLE_FRIEND, fromUserName, toUserName, fromUserName, toUserName);
	if (m_sqlManager.hasData(m_query)) 
	{
		printf("你已经添加好友[%s]，请勿重复添加\n", toUserName);
		return false;
	}
	sprintf(m_query, "INSERT INTO %s VALUES('%s', '%s')", TABLE_FRIEND, fromUserName, toUserName);
	if (m_sqlManager.ExecuteSql(m_query))
	{
		printf("添加好友[%s]成功\n", toUserName);
		return true;
	}
	else
	{
		printf("添加好友[%s]失败\n", toUserName);
		return false;
	}
}

// 添加群聊
bool Login::addGroup(char* groupName, char* userName)
{
	// 查询该群聊是否存在
	if (!hasGroup(groupName))
		return false;

	sprintf(m_query, "SELECT 1 FROM %s WHERE group_name='%s' AND user_name='%s'", TABLE_GROUP, groupName, userName);
	if (m_sqlManager.hasData(m_query))
	{
		printf("你已经加入群聊[%s]，请勿重复加入\n", groupName);
		return false;
	}

	sprintf(m_query, "INSERT INTO %s VALUES('%s', '%s')", TABLE_GROUP, groupName, userName);
	if (m_sqlManager.ExecuteSql(m_query))
	{
		printf("添加群[%s]成功\n", groupName);
		return true;
	}
	else
	{
		printf("添加群[%s]失败\n", groupName);
		return false;
	}
}

// 创建群聊
bool Login::createGroup(char * groupName, std::vector<string> userNameList)
{
	//sprintf(m_query, "SELECT group_name FROM %s WHERE user_name='%s'", TABLE_GROUP, groupName);
	if (hasGroup(groupName))
	{
		printf("当前群名[%s]已经存在，创建失败！\n", groupName);
		return false;
	}

	for (int i = 0; i < userNameList.size();i++)
	{
		sprintf(m_query, "INSERT INTO %s VALUES('%s', '%s')", TABLE_GROUP, groupName, userNameList[i]);
		if (!m_sqlManager.ExecuteSql(m_query))
		{
			printf("添加群成员[%s]失败！\n", userNameList[i]);
		}
	}
	return true;
}

// 删除好友
bool Login::delFriend(char * fromUserName, const char * toUserName)
{
	sprintf(m_query, "DELETE FROM %s WHERE (user_name1='%s' AND user_name2='%s') OR (user_name2 ='%s' AND user_name1='%s')", TABLE_FRIEND, fromUserName, toUserName, fromUserName, toUserName);
	if (m_sqlManager.ExecuteSql(m_query))
	{
		printf("删除好友[%s]成功\n", toUserName);
		return true;
	}
	else
	{
		printf("删除好友[%s]失败\n", toUserName);
		return false;
	}
}

// 删除群聊
bool Login::delGroup(const char* groupName, char* userName)
{
	sprintf(m_query, "DELETE FROM %s WHERE group_name='%s' AND user_name='%s'", TABLE_GROUP, groupName, userName);
	if (m_sqlManager.ExecuteSql(m_query))
	{
		printf("删除群聊[%s]成功\n", groupName);
		return true;
	}
	else
	{
		printf("删除群聊[%s]失败\n", groupName);
		return false;
	}
}

// 查询是否有该用户
bool Login::hasUser(const char* userName)
{
	sprintf(m_query, "SELECT 1 FROM  %s WHERE user_name='%s' limit 1", TABLE_USER, userName);
	if (!m_sqlManager.hasData(m_query))
	{
		printf("该用户不存在！\n");
		return false;
	}
	return true;
}

// 查询是否有该群聊
bool Login::hasGroup(const char* groupName)
{
	sprintf(m_query, "SELECT 1 FROM  %s WHERE group_name='%s' limit 1", TABLE_GROUP, groupName);
	if (!m_sqlManager.hasData(m_query))
	{
		printf("该群聊不存在！\n");
		return false;
	}
	return true;
}

