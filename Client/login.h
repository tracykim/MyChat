#pragma once
#include <string>
#include "MySQLManager.h"
#include "mysql_config.h"

using namespace std;
class Login{
public:
	Login();
	~Login();
	bool loginSuccess(char* userName, char* userPwd);
	void QueryFriendList(char* userName);
	void QueryGroupList(char* userName);

private:
	MySQLManager m_sqlManager;
	ConnectionInfo info;
	char m_query[1024];
};