#pragma once
#include <string>
#include "MySQLManager.h"
using namespace std;
class Login{
public:
	Login();
	~Login();
	bool loginSuccess(char* userName, char* userPwd);
private:
	MySQLManager m_sqlManager;
	ConnectionInfo info;
	char m_query[128];
};