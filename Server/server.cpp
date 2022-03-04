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

	/*serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = SERVER_PORT;*/
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
	//socksArr.push_back(listener);//�������׽��ּ����׽������飬�������׸��׽��־��Ƿ��������׽���
	//m_recvLogins.push_back(true); // ����������Ҫ��½
	m_clientInfo.emplace_back(listener, "", true);
}


void server::processSingleChat()
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

		struct timeval timeout = { 1,0 };//ÿ��Select�ȴ�1��
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
					//socksArr.push_back(clientfd);
					//m_recvLogins.push_back(false);
					m_clientInfo.emplace_back(clientfd, "", false);
					printf("connect sucessfully\n");
					char ID[1024];
					sprintf(ID, "You ID is: %d and ", clientfd);
					char buf[30] = "welcome joint the chatroom! \n";
					strcat(ID, buf);
					send(clientfd, ID, sizeof(ID) - 1, 0);//��ȥ���һ��'/0'
				}
				if (i != 0 && FD_ISSET(m_clientInfo[i].clientArr, &fds)) //socksArr[i]
				{
					char buf[1024];
					memset(buf, '\0', sizeof(buf));

					// ���տͻ��˵���Ϣ
					//int size = recv(socksArr[i], buf, sizeof(buf) - 1, 0);
					int size = recv(m_clientInfo[i].clientArr, buf, sizeof(buf) - 1, 0);
					if (buf[0] == '\0')
						continue;
					if (size > 0 && !m_clientInfo[i].recvLogin) //(!m_recvLogins[i]) // δ�յ���i���ͻ��˵�¼��Ϣ
					{				
						//m_name_arr.insert({ buf, socksArr[i] });
						//m_recvLogins[i] = true;
						
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
						//printf("clint %d says: %s \n", m_clientInfo[i].clientArr, buf);
						//char info[1024]; // ˵���Ŀͻ�������
						//sprintf(info, "client %d:", m_clientInfo[i].clientArr);
						//strcat(info, buf);

						//���˿ͻ���˵�Ļ����͸�ÿһ���ͻ���
						/*for (int j = 1; j < mount; j++)
						{
						send(socksArr[j], info, sizeof(info) - 1, 0);
						}*/

						char toUserName[128], message[1024];
						memset(toUserName, '\0', sizeof(toUserName));
						memset(message, '\0', sizeof(message));
						getUserAndMessage(buf, toUserName, message);					

						if (m_name_arr.find(toUserName) != m_name_arr.end())
						{
							char sendInfo[1024];
							memset(sendInfo, '\0', sizeof(sendInfo));
							// �ҵ������ߵ�����
							std::string fromUserName = m_clientInfo[i].clientName;
							sprintf(sendInfo, "From %s��", fromUserName.c_str());
							strcat(sendInfo, message);						
							send(m_name_arr[toUserName], sendInfo, sizeof(sendInfo) - 1, 0);
						}
												
					}
				}
			}
			break;
		}
		}
	}
}

// ���ݿո�ָ��ַ���
void getUserAndMessage(const char buf[1024], char userName[128], char message[128])
{
	int i = 0, p = 0, q=0;
	// ����û���
	while(buf[i]!='\0' && buf[i] != ' ')
	{
		userName[p++] = buf[i++];
	}
	userName[i] = '\0';
	i++;
	// �����Ϣ
	while (buf[i] != '\0')
	{
		message[q++] = buf[i++];
	}
	message[q] = '\0';
}