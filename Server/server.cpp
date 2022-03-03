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
	socksArr.push_back(listener);//�������׽��ּ����׽������飬�������׸��׽��־��Ƿ��������׽���
	m_recvLogins.push_back(true); // ����������Ҫ��½

	//socksArr.insert({ "NULL", listener });
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
		mount = socksArr.size();
		//ÿ��ѭ������һ��fds����
		for (int i = 0; i < mount; ++i)
		{
			FD_SET(socksArr[i], &fds);
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
				if (i==0 && FD_ISSET(socksArr[i], &fds))
				{
					struct sockaddr_in client_addr;
					int clntSz = sizeof(struct sockaddr_in);
					//����һ���û����׽���
					int clientfd = accept(listener, (struct sockaddr*)&client_addr, &clntSz);
					//����û�������������ʾ��Ϣ����֪ͨ�û����ӳɹ�
					socksArr.push_back(clientfd);
					m_recvLogins.push_back(false);
					printf("connect sucessfully\n");
					char ID[1024];
					sprintf(ID, "You ID is: %d and ", clientfd);
					char buf[30] = "welcome joint the chatroom! \n";
					strcat(ID, buf);
					send(clientfd, ID, sizeof(ID) - 1, 0);//��ȥ���һ��'/0'
				}
				if (i != 0 && FD_ISSET(socksArr[i], &fds))
				{
					char buf[1024];
					memset(buf, '\0', sizeof(buf));

					if (!m_recvLogins[i]) // δ�յ���i���ͻ��˵�¼��Ϣ
					{
						//User tmp;
						int nlen = recv(socksArr[i], buf, sizeof(buf) - 1, 0);
						if (nlen > 0)
						{
							m_clientInfo.insert({ buf, socksArr[i] });
							m_recvLogins[i] = true;
						}
						continue;
					}
					
					// ���տͻ��˵���Ϣ
					int size = recv(socksArr[i], buf, sizeof(buf) - 1, 0);
					//����Ƿ����
					if (size == 0 || size == -1)
					{
						printf("client %d leave\n", socksArr[i]);

						closesocket(socksArr[i]);//�ر�����׽���
						FD_CLR(socksArr[i], &fds);//���б���ɾ��
						socksArr.erase(socksArr.begin() + i);//���׽���������ɾ��
						m_recvLogins.erase(m_recvLogins.begin() + i);
					}
					//����û�е���
					else
					{
						//if (!m_recvLogins[i]) // δ�յ���i���ͻ��˵�¼��Ϣ
						//{
						//	// ���ÿͻ����û������׽���
						//	m_clientInfo.insert({ buf, socksArr[i] });
						//	m_recvLogins[i] = true;
						//}
						//else
						//{
							printf("clint %d says: %s \n", socksArr[i], buf);
							char info[1024]; // ˵���Ŀͻ�������
							sprintf(info, "client %d:", socksArr[i]);
							strcat(info, buf);

							//���˿ͻ���˵�Ļ����͸�ÿһ���ͻ���
							/*for (int j = 1; j < mount; j++)
							{
							send(socksArr[j], info, sizeof(info) - 1, 0);
							}*/

							char userName[128], message[128];
							getUserAndMessage(buf, userName, message);
							std::string userName_tmp = userName;
							if(m_clientInfo.find(userName_tmp)!=m_clientInfo.end())
								send(m_clientInfo[userName_tmp], message, sizeof(message)-1, 0);
						//}						
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
	while(buf[i] != ' ')
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