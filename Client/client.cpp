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

	
	fd_set fdread, fedwrite;
	FD_ZERO(&fdread);//将fdread清零
	FD_ZERO(&fedwrite);//将fedwrite清零

	init();

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

	if (strcmp(sendbuf, "h") == 0)
	{
		printHelp();
	}
	else if (strcmp(sendbuf, "l") == 0) // 查看好友和群列表
	{
		m_login.QueryChatList(m_user.m_userName);
	}
	else if (strcmp(sendbuf, "q") == 0)
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
	printf("h：查看帮助\n");
	printf("lf：查看好友列表\n");
	printf("lg：查看群聊列表\n");
	printf("输入好友或者群聊名字进行聊天，输入c关闭聊天窗口");
	printf("q：退出客户端\n");
}

bool client::messageIsVaild()
{
	return true;
}