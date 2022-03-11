#define _CRT_SECURE_NO_WARNINGS
#include "useMySQL.h"
UseDB::UseDB()
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
UseDB::~UseDB()
{
	m_sqlManager.FreeConnect();
}
// �����ݿ��в����Ƿ��и��û�
bool UseDB::loginSuccess(char* userName, char* userPwd)
{
	sprintf(m_query, "SELECT 1 FROM %s WHERE user_name='%s' and user_pwd='%s' limit 1", TABLE_USER, userName, userPwd);
	if (m_sqlManager.hasData(m_query)) // �û�����������ȷ����½�ɹ�
	{
		return true;
	}
	else
	{
		sprintf(m_query, "select * from %s where user_name='%s'", TABLE_USER, userName);
		if (m_sqlManager.hasData(m_query)) {
			printf("�û����������벻��ȷ\n");
			return false;
		}
		else {// ע�����û�
			sprintf(m_query, "INSERT INTO %s VALUES (NULL, '%s', '%s')", TABLE_USER, userName, userPwd);
			m_sqlManager.ExecuteSql(m_query);
			return true;
		}
	}

}

// ��ѯ�����б�
void UseDB::QueryFriendList(char* userName)
{
	sprintf(m_query, "SELECT user_name2 FROM %s WHERE user_name1='%s' UNION ALL SELECT user_name1 FROM %s WHERE user_name2 ='%s'", TABLE_FRIEND, userName, TABLE_FRIEND, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.PrintQueryRes();
}

// ��ѯ�����б����洢
void UseDB::QueryFriendList(char* userName, std::vector<string>& friendList)
{
	sprintf(m_query, "SELECT user_name2 FROM %s WHERE user_name1='%s' UNION ALL SELECT user_name1 FROM %s WHERE user_name2 ='%s'", TABLE_FRIEND, userName, TABLE_FRIEND, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.SaveQueryRes(friendList);
}

// ��ѯȺ���б�
void UseDB::QueryGroupList(char* userName)
{
	sprintf(m_query, "SELECT group_name FROM %s WHERE user_name='%s'", TABLE_GROUP, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.PrintQueryRes();
}

// ��ѯȺ���б����洢
void UseDB::QueryGroupList(char* userName, std::vector<string>& groupList)
{
	sprintf(m_query, "SELECT group_name FROM %s WHERE user_name='%s'", TABLE_GROUP, userName);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.SaveQueryRes(groupList);
}

// ��ѯȺ���е������û����洢
void UseDB::QueryUserListInGroup(char * group_name, std::vector<string>& userList)
{
	sprintf(m_query, "SELECT user_name FROM %s WHERE group_name='%s'", TABLE_GROUP, group_name);
	m_sqlManager.QueryData(m_query);
	m_sqlManager.SaveQueryRes(userList);
}
void UseDB::QueryChatList(char* userName)
{
	printf("�����б���");
	QueryFriendList(userName);
	printf("Ⱥ���б���");
	QueryGroupList(userName);
}

// ���Ӻ���
bool UseDB::addFriend(char * fromUserName, char * toUserName)
{
	// ��ѯ�ú����Ƿ����
	if (!hasUser(toUserName))
		return false;

	sprintf(m_query, "SELECT 1 FROM %s WHERE (user_name1='%s' AND user_name2='%s') OR (user_name2='%s' AND user_name1='%s')", TABLE_FRIEND, fromUserName, toUserName, fromUserName, toUserName);
	if (m_sqlManager.hasData(m_query))
	{
		printf("���Ѿ����Ӻ���[%s]�������ظ�����\n", toUserName);
		return false;
	}
	sprintf(m_query, "INSERT INTO %s VALUES('%s', '%s')", TABLE_FRIEND, fromUserName, toUserName);
	if (m_sqlManager.ExecuteSql(m_query))
	{
		printf("���Ӻ���[%s]�ɹ�\n", toUserName);
		return true;
	}
	else
	{
		printf("���Ӻ���[%s]ʧ��\n", toUserName);
		return false;
	}
}

// ����Ⱥ��
bool UseDB::addGroup(char* groupName, char* userName)
{
	// ��ѯ��Ⱥ���Ƿ����
	if (!hasGroup(groupName))
	{
		printf("��Ⱥ�Ĳ����ڣ�\n");
		return false;
	}

	sprintf(m_query, "SELECT 1 FROM %s WHERE group_name='%s' AND user_name='%s'", TABLE_GROUP, groupName, userName);
	if (m_sqlManager.hasData(m_query))
	{
		printf("���Ѿ�����Ⱥ��[%s]�������ظ�����\n", groupName);
		return false;
	}

	sprintf(m_query, "INSERT INTO %s VALUES('%s', '%s')", TABLE_GROUP, groupName, userName);
	if (m_sqlManager.ExecuteSql(m_query))
	{
		printf("����Ⱥ[%s]�ɹ�\n", groupName);
		return true;
	}
	else
	{
		printf("����Ⱥ[%s]ʧ��\n", groupName);
		return false;
	}
}

// ����Ⱥ��
bool UseDB::createGroup(char * groupName, std::vector<string> userNameList)
{
	//sprintf(m_query, "SELECT group_name FROM %s WHERE user_name='%s'", TABLE_GROUP, groupName);
	//memset(m_query, '\0', sizeof(m_query));
	if (hasGroup(groupName))
	{
		printf("��ǰȺ��[%s]�Ѿ����ڣ�����ʧ�ܣ�\n", groupName);
		return false;
	}

	for (int i = 0; i < userNameList.size(); i++)
	{
		sprintf(m_query, "INSERT INTO %s VALUES('%s', '%s')", TABLE_GROUP, groupName, userNameList[i].c_str());
		if (!m_sqlManager.ExecuteSql(m_query))
		{
			printf("����Ⱥ��Ա[%s]ʧ�ܣ�\n", userNameList[i]);
		}
	}

	printf("����Ⱥ�ĳɹ���\n");
	return true;
}

// ɾ������
bool UseDB::delFriend(char * fromUserName, const char * toUserName)
{
	sprintf(m_query, "DELETE FROM %s WHERE (user_name1='%s' AND user_name2='%s') OR (user_name2 ='%s' AND user_name1='%s')", TABLE_FRIEND, fromUserName, toUserName, fromUserName, toUserName);
	if (m_sqlManager.ExecuteSql(m_query))
	{
		printf("ɾ������[%s]�ɹ�\n", toUserName);
		return true;
	}
	else
	{
		printf("ɾ������[%s]ʧ��\n", toUserName);
		return false;
	}
}

// ɾ��Ⱥ��
bool UseDB::delGroup(const char* groupName, char* userName)
{
	sprintf(m_query, "DELETE FROM %s WHERE group_name='%s' AND user_name='%s'", TABLE_GROUP, groupName, userName);
	if (m_sqlManager.ExecuteSql(m_query))
	{
		printf("ɾ��Ⱥ��[%s]�ɹ�\n", groupName);
		return true;
	}
	else
	{
		printf("ɾ��Ⱥ��[%s]ʧ��\n", groupName);
		return false;
	}
}

// ��ѯ�Ƿ��и��û�
bool UseDB::hasUser(const char* userName)
{
	sprintf(m_query, "SELECT 1 FROM  %s WHERE user_name='%s' limit 1", TABLE_USER, userName);
	if (!m_sqlManager.hasData(m_query))
	{
		printf("���û������ڣ�\n");
		return false;
	}
	return true;
}

// ��ѯ�Ƿ��и�Ⱥ��
bool UseDB::hasGroup(const char* groupName)
{
	sprintf(m_query, "SELECT 1 FROM  %s WHERE group_name='%s' limit 1", TABLE_GROUP, groupName);
	if (!m_sqlManager.hasData(m_query))
	{
		return false;
	}
	return true;
}
