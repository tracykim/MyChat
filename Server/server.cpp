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

	/*serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = SERVER_PORT;*/
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
	//socksArr.push_back(listener);//将监听套接字加入套接字数组，数组内首个套接字就是服务器的套接字
	//m_recvLogins.push_back(true); // 服务器不需要登陆
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

		struct timeval timeout = { 1,0 };//每个Select等待1秒
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
					//socksArr.push_back(clientfd);
					//m_recvLogins.push_back(false);
					m_clientInfo.emplace_back(clientfd, "", false);
					printf("connect sucessfully\n");
					
					//char ID[1024];
					//sprintf(ID, "You ID is: %d and ", clientfd);
					//char buf[30] = "welcome joint the chatroom! \n";
					//strcat(ID, buf);
					//send(clientfd, ID, sizeof(ID) - 1, 0);//减去最后一个'/0'
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
						//m_name_arr.insert({ buf, socksArr[i] });
						//m_recvLogins[i] = true;
						
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
						//printf("clint %d says: %s \n", m_clientInfo[i].clientArr, buf);
						//char info[1024]; // 说话的客户端名字
						//sprintf(info, "client %d:", m_clientInfo[i].clientArr);
						//strcat(info, buf);

						//char toUserName[128], message[1024];
						//memset(toUserName, '\0', sizeof(toUserName));
						//memset(message, '\0', sizeof(message));
						//getUserAndMessage(buf, toUserName, message);					

						//if (m_name_arr.find(toUserName) != m_name_arr.end())
						//{
						//	char sendInfo[1024];
						//	memset(sendInfo, '\0', sizeof(sendInfo));
						//	// 找到发送者的名字
						//	std::string fromUserName = m_clientInfo[i].clientName;
						//	sprintf(sendInfo, "From %s：", fromUserName.c_str());
						//	strcat(sendInfo, message);						
						//	send(m_name_arr[toUserName], sendInfo, sizeof(sendInfo) - 1, 0);
						//}

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
							//sprintf(sendInfo, "用户[%s]添加你为好友", fromName);
							send(m_name_arr[toName], buf, sizeof(buf) - 1, 0);
							break;
						case '1': // 1、加群聊
							sprintf(sendInfo, "用户[%s]加入群聊[%s]", fromName, toName);
							sendGroupMessage(sendInfo, toName, fromName);
							break;
						case '2': // 2、创建群聊
							//sprintf(sendInfo, "用户[%s]拉你进入群聊[%s]：", fromName, toName);
							//sendGroupMessage(buf, toName, fromName);
							if (m_db.hasGroup(toName))
							{
								// 查询群聊用户
								std::vector<string> groupList;
								m_db.QueryUserListInGroup(toName, groupList);
								for (int i = 0; i < groupList.size(); i++)
								{
									if (groupList[i] != fromName)
									{
										if (m_name_arr.find(groupList[i]) != m_name_arr.end())
										{											
											send(m_name_arr[groupList[i]], buf, sizeof(buf) - 1, 0);
										}
									}
								}
							}
							break;
						case '3': // 3、删除好友
							//sprintf(sendInfo, "你被用户[%s]删除好友：", fromName);
							send(m_name_arr[toName], buf, sizeof(sendInfo) - 1, 0);
							break;
						case '4': // 4、删除群聊
							sprintf(sendInfo, "用户[%s]退出群聊：", fromName);
							sendGroupMessage(sendInfo, toName, fromName);
							break;
						case '5': // 5、单聊
							sendUser(data, fromName, toName);
							break;
						case '6': // 6、群聊
							sprintf(sendInfo, "[%s]-[%s]：", toName, fromName);
							strcat(sendInfo, data);
							sendGroupMessage(sendInfo, toName, fromName);

							//if (m_db.hasGroup(toName))
							//{
							//	// 查询群聊用户
							//	std::vector<string> groupList;
							//	m_db.QueryUserListInGroup(toName, groupList);
							//	for (int i = 0; i < groupList.size(); i++)
							//	{
							//		if (groupList[i] != fromName)
							//		{
							//			if (m_name_arr.find(groupList[i]) != m_name_arr.end())
							//			{											
							//				send(m_name_arr[groupList[i]], sendInfo, sizeof(sendInfo) - 1, 0);
							//			}
							//		}
							//	}
							//}
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

// 处理客户端发过来的消息
/*void server::processMessage(const char* buf, int clinetIdx)
{
	char name[128], message[1024];
	memset(name, '\0', sizeof(name));
	memset(message, '\0', sizeof(message));
	getUserAndMessage(buf, name, message);

	// 不是用户名或当前用户不在线
	if (!sendMessage(message, m_clientInfo[clinetIdx].clientName, name))
	{
		if (m_db.hasGroup(name)) // 如果发送的是群聊名，进行群聊
		{
			// 查询群聊用户
			std::vector<string> groupList;
			m_db.QueryUserListInGroup(name, groupList);
			for (int i = 0; i < groupList.size(); i++)
			{
				if(groupList[i]!=m_clientInfo[clinetIdx].clientName)
					sendMessage(message, name, m_clientInfo[clinetIdx].clientName, groupList[i]);
			}
		}
	}
}*/

void server::processData(char& dataType, char fromName[128], char toName[128], char data[1024], const char* buf)
{
	dataType = buf[0];
	int i = 2, p = 0, q = 0, r=0;
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

// 一个客户端给另一个客户端发送消息
bool server::sendUser(char message[1024], std::string fromClinetName, std::string toClinetName)
{
	// 当前客户端是否在线在线
	if (m_name_arr.find(toClinetName) != m_name_arr.end())
	{
		char sendInfo[1024];
		//memset(sendInfo, '\0', sizeof(sendInfo));
		sprintf(sendInfo, "[%s]：", fromClinetName.c_str());
		strcat(sendInfo, message);
		send(m_name_arr[toClinetName], sendInfo, sizeof(sendInfo) - 1, 0);
		return true;
	}
	return false;
}

// 在群聊中一个客户端给另一个客户端发消息
bool server::sendUserByGroup(char message[1024], char groupName[128], std::string fromClinetName, std::string toClinetName)
{
	// 当前客户端是否在线在线
	if (m_name_arr.find(toClinetName) != m_name_arr.end())
	{
		//char sendInfo[1024];
		//sprintf(sendInfo, "[%s]-[%s]：", groupName, fromClinetName.c_str());
		//strcat(sendInfo, message);
		//send(m_name_arr[toClinetName], sendInfo, sizeof(sendInfo) - 1, 0);
		send(m_name_arr[toClinetName], message, sizeof(message)-1, 0);
		return true;
	}
	return false;
}

// 给群聊中的每个客户端发消息 TODO 这个函数调用有问题，展开来写没问题
bool server::sendGroupMessage(char message[1024], char groupName[128], std::string fromClinetName)
{
	if (m_db.hasGroup(groupName))
	{
		// 查询群聊用户
		std::vector<string> groupList;
		m_db.QueryUserListInGroup(groupName, groupList);
		for (int i = 0; i < groupList.size(); i++)
		{
			if (groupList[i] != fromClinetName)
				sendUserByGroup(message, groupName, fromClinetName, groupList[i]);
		}
		return true;
	}
	return false;
}

// 根据空格分隔字符串
//void getUserAndMessage(const char buf[1024], char userName[128], char message[128])
//{
//	int i = 0, p = 0, q=0;
//	// 填充用户名
//	while(buf[i]!='\0' && buf[i] != ' ')
//	{
//		userName[p++] = buf[i++];
//	}
//	userName[p] = '\0';
//	i++;
//	// 填充信息
//	while (buf[i] != '\0')
//	{
//		message[q++] = buf[i++];
//	}
//	message[q] = '\0';
//}