#pragma once
#include <string>
#include <vector>
#include "MySQLManager.h"
#include "mysql_config.h"

using namespace std;
class Login{
public:
	Login();
	~Login();
	bool loginSuccess(char* userName, char* userPwd);
	void QueryFriendList(char* userName);
	void QueryFriendList(char* userName, std::vector<string>& friendList);

	void QueryGroupList(char* userName);
	void QueryGroupList(char* userName, std::vector<string>& groupList);

	void QueryChatList(char* userName);
	bool addFriend(char * fromUserName, char * toUserName);
	bool addGroup(char* groupName, char* userName);
	bool createGroup(char* groupName, std::vector<string> userNameList);
	bool delFriend(char * fromUserName, const char * toUserName);
	bool delGroup(const char* groupName, char* userName);

private:
	MySQLManager m_sqlManager;
	ConnectionInfo info;
	char m_query[1024];
	bool hasUser(const char* userName);
	bool hasGroup(const char* groupName);
};