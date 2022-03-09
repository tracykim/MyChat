#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "client.h"

client::client()
{
	m_handle = 0;
	writing = 0;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_HOST);//将字符串类型转换uint32_t
}

void client::init()
{
	WSADATA   wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_handle = socket(AF_INET, SOCK_STREAM, 0);//采用ipv4,TCP传输，成功时返回非负数socket描述符

	if (m_handle <= 0)
	{
		perror("establish client faild");
		printf("Error at socket(): %ld\n", WSAGetLastError());
		exit(1);
	};
	printf("establish succesfully\n");//创建成功，阻塞式的等待服务器连接
	if (connect(m_handle, (const sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("connect to server faild");
		printf("Error at socket(): %ld\n", WSAGetLastError());
		exit(1);
	}
	printf("connect IP:%s  Port:%d succesfully\n", SERVER_HOST, SERVER_PORT);//创建成功
	//memset(m_user.m_userName, '\0', sizeof(m_user.m_userName));
	//memset(m_user.m_userPwd, '\0', sizeof(m_user.m_userPwd));
}

void client::process()
{
	fd_set fdread, fedwrite;
	FD_ZERO(&fdread);//将fdread清零
	FD_ZERO(&fedwrite);//将fedwrite清零

	init();

	/// 登录	
	printf("欢迎来到我的聊天室，请登录.\n");

	printf("用户名：");
	scanf("%s", &m_user.m_userName);
	printf("密码：");
	scanf("%s", &m_user.m_userPwd);

	while (!m_login.loginSuccess(m_user.m_userName, m_user.m_userPwd)) {
		printf("请重新输入.\n");
		printf("用户名：");
		scanf("%s", &m_user.m_userName);
		printf("密码：");
		scanf("%s", &m_user.m_userPwd);
	}

	printf("登录成功！\n");
	printHelp();

	/// 存储用户的好友和群列表
	m_login.QueryFriendList(m_user.m_userName, m_friendList);
	m_login.QueryGroupList(m_user.m_userName, m_groupList);

	send(m_handle, m_user.m_userName, sizeof(m_user.m_userName) - 1, 0);
	while (!m_quit)
	{
		FD_SET(m_handle, &fdread);
		if (writing == 0)
			FD_SET(m_handle, &fedwrite);
		struct timeval timeout = { 1,0 };//每个Select等待1秒
		switch (select(0, &fdread, &fedwrite, NULL, &timeout))
		{
		case -1:
		{
			printf("Error at socket(): %ld\n", WSAGetLastError());
			break;
		}
		case 0:
			break;
		default:
		{
			if (FD_ISSET(m_handle, &fdread))//有待读事件
			{
				char recvbuf[1024];
				memset(recvbuf, '\0', sizeof(recvbuf));
				int size = recv(m_handle, recvbuf, sizeof(recvbuf) - 1, 0);
				if (size > 0)
				{
					printf("server : %s\n", recvbuf);
					memset(recvbuf, '\0', sizeof(recvbuf));
				}
				else if (size == 0)
				{
					printf("server is closed\n");
					exit(1);
				}
			}
			if (FD_ISSET(m_handle, &fedwrite))//有待写事件
			{
				FD_ZERO(&fedwrite);//将fedwrite清零
				writing = 1;//表示正在写入
				std::thread sendtask(std::bind(&client::sendata, this));
				sendtask.detach();//将子线程和主进程分离且互相不影响
			}
			break;
		}
		}
	}
}

void client::sendata()
{
	char sendbuf[1024];
	memset(sendbuf, '\0', sizeof(sendbuf));
	std::cin.getline(sendbuf, 1024);//读取一行

	if (strcmp(sendbuf, "--h") == 0 || strcmp(sendbuf, "--help") == 0)
	{
		printHelp();
	}
	else if (strcmp(sendbuf, "--ls") == 0) // 查看好友和群列表
	{
		//m_login.QueryChatList(m_user.m_userName);
		printFrinedList();
		printGroupList();
	}
	else if (strcmp(sendbuf, "--addu") == 0) // 添加好友
	{
		printf("请输入要添加的好友：");
		char toUserName[128];
		memset(toUserName, '\0', sizeof(toUserName));
		scanf("%s", &toUserName);
		if (m_login.addFriend(m_user.m_userName, toUserName))
		{
			m_friendList.push_back(toUserName);
			// 给好友发添加好友的消息

		}
	}
	else if (strcmp(sendbuf, "--addg") == 0)// 添加群聊
	{
		printf("请输入要添加的群名：");
		char groupName[128];
		scanf("%s", &groupName);
		if(m_login.addGroup(groupName, m_user.m_userName))
			m_groupList.push_back(groupName);
	}
	else if (strcmp(sendbuf, "--createg") == 0)// 创建群聊
	{
		printf("请输入要创建的群名：");
		char groupName[128];
		scanf("%s", &groupName);
		printFrinedList();
		printf("请输入要添加的群成员编号（换行结束）：");
		std::vector<string> addList;
		addList.push_back(m_user.m_userName);
		int idx;
		while (cin>>idx)
		{
			if (idx<0 || idx>m_friendList.size() - 1)
				printf("输入编号无效！\n");
			else
				addList.push_back(m_friendList[idx]);
			if (cin.get() == '\n')
				break;
		}
		if (addList.size() < 3)
		{
			printf("创建群至少选择三个成员!\n");
		}
		if (m_login.createGroup(groupName, addList))
			m_groupList.push_back(groupName);
	}
	else if (strcmp(sendbuf, "--delu") == 0) // 删除好友
	{
		printFrinedList();
		printf("请输入要删除的好友编号：");
		int delNum;
		scanf("%d", &delNum);
		if (delNum<0 || delNum>m_friendList.size() - 1)
			printf("输入编号无效！\n");
		else
		{
			if(m_login.delFriend(m_user.m_userName, m_friendList[delNum].c_str()))
				m_friendList.erase(m_friendList.begin() + delNum);
		}
	}
	else if (strcmp(sendbuf, "--delg") == 0) // 删除群聊
	{
		printGroupList();
		printf("请输入要删除的群聊编号：");
		int delNum;
		scanf("%d", &delNum);
		if (delNum<0 || delNum>m_groupList.size() - 1)
			printf("输入编号无效！");
		else
		{
			if(m_login.delGroup(m_groupList[delNum].c_str(), m_user.m_userName))
				m_groupList.erase(m_groupList.begin() + delNum);
		}
	}
	else if (strcmp(sendbuf, "--q") == 0)
	{
		m_quit = true;
	}
	else
	{	
		if(messageIsVaild()) // 查看输入信息的合法性
			send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);
	}
	writing = 0;	
}

void client::printHelp()
{
	printf("--h：查看帮助\n");
	printf("--ls：查看好友和群列表\n");
	printf("--addu：添加好友\n");
	printf("--addg：添加群聊\n");
	printf("--createg：创建群聊\n");
	printf("--delu：删除好友\n");
	printf("--delg：删除群聊\n");
	//printf("输入好友或者群聊名字进行聊天，输入c关闭聊天窗口");
	printf("--q：退出客户端\n\n");
}

bool client::messageIsVaild()
{
	return true;
}

void client::printFrinedList()
{
	printf("好友列表：");
	for (int i = 0; i < m_friendList.size();i++)
	{
		printf("\t%d：%5s", i, m_friendList[i].c_str());
	}
	printf("\n");
}

void client::printGroupList()
{
	printf("群聊列表：");
	for (int i = 0; i < m_groupList.size(); i++)
	{
		printf("\t%d：%5s", i, m_groupList[i].c_str());
	}
	printf("\n");
}