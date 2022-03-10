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
					//printf("%s\n", recvbuf);
					
					/// 对收到的数据进行解析
					char dataType;
					char fromName[128], toName[128], data[1024];
					memset(fromName, '\0', sizeof(fromName));
					memset(toName, '\0', sizeof(toName));
					memset(data, '\0', sizeof(data));

					processData(dataType, fromName, toName, data, recvbuf);
					char sendInfo[1024];
					switch (dataType)
					{
					case '0': // 0、被加好友
						printf("用户[%s]添加你为好友\n", fromName);
						m_friendList.push_back(fromName);
						break;
					//case '1': // 1、加群聊
					//	break;
					case '2': // 2、被拉入群聊
						printf("用户[%s]拉你进入群聊[%s]\n", fromName, toName);
						m_groupList.push_back(toName);
						break;
					case '3': // 3、被删除好友
					{
						printf("你被用户[%s]删除好友\n", fromName);
						int i = 0;
						for (; i < m_friendList.size(); i++)
						{
							if (m_friendList[i] == fromName)
								break;
						}
						if (i != m_friendList.size() && !m_friendList.empty())
						{
							m_friendList.erase(m_friendList.begin() + i);
						}
						break;
					}
					//case '4': // 4、删除群聊
					//	sprintf(sendInfo, "用户[%s]退出群聊：", fromName);
					//	sendGroupMessage(sendInfo, toName, fromName);
					//	break;
					//case '5': // 5、单聊
					//	sendMessage(data, fromName, toName);
					//	break;
					//case '6': // 6、群聊
					//	sendGroupMessage(data, toName, fromName);
					//	break;
					default:
						printf("%s\n", recvbuf);
						break;
					}
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
	else if (strcmp(sendbuf, "--addu") == 0) // 0、添加好友
	{
		printf("请输入要添加的好友：");
		char toUserName[128];
		scanf("%s", &toUserName);
		if (m_login.addFriend(m_user.m_userName, toUserName))
		{
			m_friendList.push_back(toUserName);
			splitData(0, m_user.m_userName, toUserName, "\0", sendbuf);
			send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);
		}
	}
	else if (strcmp(sendbuf, "--addg") == 0)// 1、添加群聊
	{
		printf("请输入要添加的群名：");
		char groupName[128];
		scanf("%s", &groupName);
		if (m_login.addGroup(groupName, m_user.m_userName))
		{
			// 给群聊发加入新成员的消息
			splitData(1, m_user.m_userName, groupName, "\0", sendbuf);
			send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);
			m_groupList.push_back(groupName);
		}
	}
	else if (strcmp(sendbuf, "--createg") == 0)// 2、创建群聊
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
		{
			splitData(2, m_user.m_userName, groupName, "\0", sendbuf);
			send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);
			m_groupList.push_back(groupName);
		}
	}
	else if (strcmp(sendbuf, "--delu") == 0) // 3、删除好友
	{
		printFrinedList();
		printf("请输入要删除的好友编号：");
		int delNum;
		scanf("%d", &delNum);
		if (delNum<0 || delNum>m_friendList.size() - 1)
			printf("输入编号无效！\n");
		else
		{
			if (m_login.delFriend(m_user.m_userName, m_friendList[delNum].c_str()))
			{
				splitData(3, m_user.m_userName, m_friendList[delNum].c_str(), "\0", sendbuf);
				send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);
				m_friendList.erase(m_friendList.begin() + delNum);
			}
		}
	}
	else if (strcmp(sendbuf, "--delg") == 0) // 4、删除群聊
	{
		printGroupList();
		printf("请输入要删除的群聊编号：");
		int delNum;
		scanf("%d", &delNum);
		if (delNum<0 || delNum>m_groupList.size() - 1)
			printf("输入编号无效！");
		else
		{
			if (m_login.delGroup(m_groupList[delNum].c_str(), m_user.m_userName))
			{
				splitData(4, m_user.m_userName, m_groupList[delNum].c_str(), "\0", sendbuf);
				send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);
				m_groupList.erase(m_groupList.begin() + delNum);
			}
		}
	}
	else if (strcmp(sendbuf, "--chatu") == 0) // 5、单聊
	{
		printFrinedList();
		printf("请输入要聊天的好友编号：");
		int num;
		scanf("%d", &num);
		if (num<0 || num>m_friendList.size() - 1)
			printf("输入编号无效！\n");
		else
		{		
			printf(">[%s]：", m_friendList[num].c_str());
			char message[1024];
			cin.get(); // 为cin.getline刷新缓存区
			std::cin.getline(message, 1024);
			splitData(5, m_user.m_userName, m_friendList[num].c_str(), message, sendbuf);
			send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);						
		}
	}
	else if (strcmp(sendbuf, "--chatg") == 0) // 6、群聊
	{
		printGroupList();
		printf("请输入要聊天的群聊编号：");
		int num;
		scanf("%d", &num);
		if (num<0 || num>m_groupList.size() - 1)
			printf("输入编号无效！\n");
		else
		{
			printf(">[%s]：", m_groupList[num].c_str());
			char message[1024];
			cin.get();
			std::cin.getline(message, 1024);
			splitData(6, m_user.m_userName, m_groupList[num].c_str(), message, sendbuf);
			send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);
		}
	}
	else if (strcmp(sendbuf, "--q") == 0)
	{
		m_quit = true;
	}
	//else
	//{	
	//	if(messageIsVaild()) // 查看输入信息的合法性
	//		send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);
	//}
	writing = 0;	
}

void client::printHelp()
{
	printf("--h：		1.查看帮助\n");
	printf("--ls：		2.查看好友和群列表\n");
	printf("--addu：	3.添加好友\n");
	printf("--addg：	4.添加群聊\n");
	printf("--createg   5.创建群聊\n");
	printf("--delu：	6.删除好友\n");
	printf("--delg：	7.删除群聊\n");
	printf("--chatu：	8.单聊\n");
	printf("--chatg：	9.群聊\n");
	printf("--q：		10.退出客户端\n\n");
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

void client::splitData(int dataType, const char* fromName, const char* toName, const char* data, char* sendbuf)
{
	sprintf(sendbuf, "%d ", dataType);
	strcat(sendbuf, fromName);
	strcat(sendbuf, " ");
	strcat(sendbuf, toName);
	strcat(sendbuf, " ");
	strcat(sendbuf, data);
}

void client::processData(char& dataType, char fromName[128], char toName[128], char data[1024], const char* buf)
{
	dataType = buf[0];
	int i = 2, p = 0, q = 0, r = 0;
	// 填充From
	while (buf[i] != '\0' && buf[i] != ' ')
	{
		fromName[p++] = buf[i++];
	}
	fromName[p] = '\0';
	i++;

	// 填充To
	while (buf[i] != '\0' && buf[i] != ' ')
	{
		toName[q++] = buf[i++];
	}
	toName[q] = '\0';
	i++;

	// 填充data
	while (buf[i] != '\0')
	{
		data[r++] = buf[i++];
	}
	data[r] = '\0';
}