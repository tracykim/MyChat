#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "server.h"
#include "user.h"

//���������Ϣ
server::server()
{
	listener = 0;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_HOST);//���ַ�������ת��uint32_t
}
//��ʼ�����������������׽��֣��󶨶˿ڣ������м���
void server::init()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// �����׽���
	listener = socket(AF_INET, SOCK_STREAM, 0);//����ipv4,TCP����
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
	listen(listener, 5);//listener����׽��ּ�����������ӣ����ȴ����Ӷ���Ϊ5,�ȴ�accept()
	m_clientInfo.emplace_back(listener, "", true);
}


void server::process()
{
	int mount = 0;
	fd_set fds, fds_copy;
	FD_ZERO(&fds);//��fds����

	init();
	//���ϵļ����������Ӷ���
	printf("Server is waiting......\n");
	while (1)
	{
		//mount = socksArr.size();
		mount = m_clientInfo.size();
		//ÿ��ѭ������һ��fds����
		for (int i = 0; i < mount; ++i)
		{
			//FD_SET(socksArr[i], &fds);
			FD_SET(m_clientInfo[i].clientArr, &fds);
		}

		struct timeval timeout = { 1,0 };//Select��ʱʱ�䣺1��
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
		case 0:		//�ȴ�ʱ�����޿ͻ�����������
		{
			break;
		}
		default:
		{
			//�������е�ÿһ���׽��ֶ���ʣ����׽��ֽ��бȽϣ��Ӷ��õ���ǰ������
			for (int i = 0; i < mount; ++i)
			{
				//�׸��׽��־��Ƿ��������׽��֣����ڴ��ڲ��յ��ͻ����������󣬽������Ӳ������׽������飨�����ӵĿͻ����׽��ּӽ������У�
				if (i==0 && FD_ISSET(m_clientInfo[i].clientArr, &fds)) //socksArr[i]
				{
					struct sockaddr_in client_addr;
					int clntSz = sizeof(struct sockaddr_in);
					//����һ���û����׽���
					int clientfd = accept(listener, (struct sockaddr*)&client_addr, &clntSz);
					//����û�������������ʾ��Ϣ����֪ͨ�û����ӳɹ�
					m_clientInfo.emplace_back(clientfd, "", false);
					printf("connect sucessfully\n");
				}
				if (i != 0 && FD_ISSET(m_clientInfo[i].clientArr, &fds)) //socksArr[i]
				{
					char buf[1024];
					memset(buf, '\0', sizeof(buf));

					// ���տͻ��˵���Ϣ
					//int size = recv(socksArr[i], buf, sizeof(buf) - 1, 0);
					int size = recv(m_clientInfo[i].clientArr, buf, sizeof(buf) - 1, 0);
					if (size > 0 && !m_clientInfo[i].recvLogin) // δ�յ���i���ͻ��˵�¼��Ϣ
					{				
						m_name_arr.insert({ buf, m_clientInfo[i].clientArr });
						m_clientInfo[i].clientName = buf;
						m_clientInfo[i].recvLogin = true;
						continue;
					}					
					
					//����Ƿ����
					if (size == 0 || size == -1)
					{
						printf("client %d leave\n", m_clientInfo[i].clientArr);
						closesocket(m_clientInfo[i].clientArr);//�ر�����׽���
						FD_CLR(m_clientInfo[i].clientArr, &fds);//���б���ɾ��
						//socksArr.erase(socksArr.begin() + i);//���׽���������ɾ��
						//m_recvLogins.erase(m_recvLogins.begin() + i);
						m_clientInfo.erase(m_clientInfo.begin() + i);
					}
					//����û�е���
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
						case '0': // 0���Ӻ���
							send(m_name_arr[toName], buf, sizeof(buf) - 1, 0);
							break;
						case '1': // 1����Ⱥ��
							sprintf(sendInfo, "�û�[%s]����Ⱥ��[%s]", fromName, toName);
							sendGroupMessage(sendInfo, sizeof(sendInfo), toName, fromName);
							break;
						case '2': // 2������Ⱥ��
							sendGroupMessage(buf, sizeof(buf), toName, fromName);
							break;
						case '3': // 3��ɾ������
							send(m_name_arr[toName], buf, sizeof(buf) - 1, 0);
							break;
						case '4': // 4��ɾ��Ⱥ��
							sprintf(sendInfo, "�û�[%s]�˳�Ⱥ��", fromName);
							sendGroupMessage(sendInfo, sizeof(sendInfo), toName, fromName);
							break;
						case '5': // 5������
							sprintf(sendInfo, "[%s]��", fromName);
							strcat(sendInfo, data);
							sendUser(sendInfo, toName, sizeof(sendInfo));
							break;
						case '6': // 6��Ⱥ��
							sprintf(sendInfo, "[%s]-[%s]��", toName, fromName);
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
	// ��ǰ�ͻ����Ƿ���������
	if (m_name_arr.find(toClinetName) != m_name_arr.end())
	{
		send(m_name_arr[toClinetName], message, messageLen-1, 0);
		return true;
	}
	return false;
}

// ��Ⱥ���е�ÿ���ͻ��˷���Ϣ TODO ����������������⣬չ����дû����
bool server::sendGroupMessage(char message[1024], int messageLen, char groupName[128], std::string fromClinetName)
{
	if (m_db.hasGroup(groupName))
	{
		// ��ѯȺ���û�
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