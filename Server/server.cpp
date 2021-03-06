#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "server.h"
#include "user.h"

//填充服务端信息
server::server()
{
	listener = 0;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_HOST);//将字符串类型转换uint32_t
}
//初始化函数，创建监听套接字，绑定端口，并进行监听
void server::init()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// 创建套接字
	listener = socket(AF_INET, SOCK_STREAM, 0);//采用ipv4,TCP传输
	if (listener == -1) { 
		printf("Error at socket(): %ld\n", WSAGetLastError()); 
		perror("listener failed"); 
		exit(1); 
	}
	else printf("Server is running......\n");

	if (::bind(listener, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("bind error");
		exit(1);
	}
	listen(listener, 5);//listener这个套接字监听申请的链接，最大等待连接队列为5,等待accept()
	m_clientInfo.emplace_back(listener, "", true);
}


void server::process()
{
	int mount = 0;
	fd_set fds, fds_copy;
	FD_ZERO(&fds);//将fds清零

	init();
	//不断的检查申请的连接队列
	printf("Server is waiting......\n");
	while (1)
	{
		//mount = socksArr.size();
		mount = m_clientInfo.size();
		//每次循环更新一次fds数组
		for (int i = 0; i < mount; ++i)
		{
			//FD_SET(socksArr[i], &fds);
			FD_SET(m_clientInfo[i].clientArr, &fds);
		}

		struct timeval timeout = { 1,0 };//Select超时时间：1秒
		switch (select(0, &fds, NULL, NULL, &timeout))
		{
		case -1:     //select error
		{
			perror("select\n");
			printf("Error at socket(): %ld\n", WSAGetLastError());
			printf("%d\n", mount);
			Sleep(1000);
			break;
		}
		case 0:		//等待时间内无客户端申请连接
		{
			break;
		}
		default:
		{
			//将数组中的每一个套接字都和剩余的套接字进行比较，从而得到当前的任务
			for (int i = 0; i < mount; ++i)
			{
				//首个套接字就是服务器的套接字，当期存在并收到客户端连接请求，进行连接并更新套接字数组（将连接的客户端套接字加进数组中）
				if (i==0 && FD_ISSET(m_clientInfo[i].clientArr, &fds)) //socksArr[i]
				{
					struct sockaddr_in client_addr;
					int clntSz = sizeof(struct sockaddr_in);
					//返回一个用户的套接字
					int clientfd = accept(listener, (struct sockaddr*)&client_addr, &clntSz);
					//添加用户，服务器上显示消息，并通知用户连接成功
					m_clientInfo.emplace_back(clientfd, "", false);
					printf("connect sucessfully\n");
				}
				if (i != 0 && FD_ISSET(m_clientInfo[i].clientArr, &fds)) //socksArr[i]
				{
					char buf[1024];
					memset(buf, '\0', sizeof(buf));

					// 接收客户端的消息
					//int size = recv(socksArr[i], buf, sizeof(buf) - 1, 0);
					int size = recv(m_clientInfo[i].clientArr, buf, sizeof(buf) - 1, 0);
					if (size > 0 && !m_clientInfo[i].recvLogin) // 未收到第i个客户端登录信息
					{				
						m_name_arr.insert({ buf, m_clientInfo[i].clientArr });
						m_clientInfo[i].clientName = buf;
						m_clientInfo[i].recvLogin = true;
						continue;
					}					
					
					//检测是否断线
					if (size == 0 || size == -1)
					{
						printf("client %d leave\n", m_clientInfo[i].clientArr);
						closesocket(m_clientInfo[i].clientArr);//关闭这个套接字
						FD_CLR(m_clientInfo[i].clientArr, &fds);//在列表中删除
						//socksArr.erase(socksArr.begin() + i);//在套接字数组中删除
						//m_recvLogins.erase(m_recvLogins.begin() + i);
						m_clientInfo.erase(m_clientInfo.begin() + i);
					}
					//若是没有掉线
					else
					{				
						if (buf[0] == '\0')
							continue;
						char dataType;
						char fromName[128], toName[128], data[1024];
						memset(fromName, '\0', sizeof(fromName));
						memset(toName, '\0', sizeof(toName));
						memset(data, '\0', sizeof(data));
						processData(dataType, fromName, toName, data, buf);

						char sendInfo[1024];
						memset(sendInfo, '\0', sizeof(sendInfo));

						switch (dataType)
						{
						case '0': // 0、加好友
							send(m_name_arr[toName], buf, sizeof(buf) - 1, 0);
							break;
						case '1': // 1、加群聊
							sprintf(sendInfo, "用户[%s]加入群聊[%s]", fromName, toName);
							sendGroupMessage(sendInfo, sizeof(sendInfo), toName, fromName);
							break;
						case '2': // 2、创建群聊
							sendGroupMessage(buf, sizeof(buf), toName, fromName);
							break;
						case '3': // 3、删除好友
							send(m_name_arr[toName], buf, sizeof(buf) - 1, 0);
							break;
						case '4': // 4、删除群聊
							sprintf(sendInfo, "用户[%s]退出群聊", fromName);
							sendGroupMessage(sendInfo, sizeof(sendInfo), toName, fromName);
							break;
						case '5': // 5、单聊
							sprintf(sendInfo, "[%s]：", fromName);
							strcat(sendInfo, data);
							sendUser(sendInfo, toName, sizeof(sendInfo));
							break;
						case '6': // 6、群聊
							sprintf(sendInfo, "[%s]-[%s]：", toName, fromName);
							strcat(sendInfo, data);
							sendGroupMessage(sendInfo, sizeof(sendInfo), toName, fromName);
							break;
						default:
							break;
						}

					}
				}
			}
			break;
		}
		}
	}
}

bool server::sendUser(char message[1024], std::string toClinetName, int messageLen)
{
	// 当前客户端是否在线在线
	if (m_name_arr.find(toClinetName) != m_name_arr.end())
	{
		send(m_name_arr[toClinetName], message, messageLen-1, 0);
		return true;
	}
	return false;
}

// 给群聊中的每个客户端发消息 TODO 这个函数调用有问题，展开来写没问题
bool server::sendGroupMessage(char message[1024], int messageLen, char groupName[128], std::string fromClinetName)
{
	if (m_db.hasGroup(groupName))
	{
		// 查询群聊用户
		std::vector<string> groupList;
		m_db.QueryUserListInGroup(groupName, groupList);
		for (int i = 0; i < groupList.size(); i++)
		{
			if (groupList[i] != fromClinetName)
				sendUser(message, groupList[i], messageLen);//sendUserByGroup(message, groupList[i]);
		}
		return true;
	}
	return false;
}