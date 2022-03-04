#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "client.h"

client::client()
{
	m_handle = 0;
	writing = 0;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_HOST);//���ַ�������ת��uint32_t
}

void client::init()
{
	WSADATA   wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_handle = socket(AF_INET, SOCK_STREAM, 0);//����ipv4,TCP���䣬�ɹ�ʱ���طǸ���socket������

	if (m_handle <= 0)
	{
		perror("establish client faild");
		printf("Error at socket(): %ld\n", WSAGetLastError());
		exit(1);
	};
	printf("establish succesfully\n");//�����ɹ�������ʽ�ĵȴ�����������
	if (connect(m_handle, (const sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("connect to server faild");
		printf("Error at socket(): %ld\n", WSAGetLastError());
		exit(1);
	}
	printf("connect IP:%s  Port:%d succesfully\n", SERVER_HOST, SERVER_PORT);//�����ɹ�
	//memset(m_user.m_userName, '\0', sizeof(m_user.m_userName));
	//memset(m_user.m_userPwd, '\0', sizeof(m_user.m_userPwd));
}

void client::process()
{
	/// ��¼	
	printf("��ӭ�����ҵ������ң����¼.\n");
	printf("�û�����");
	scanf("%s", &m_user.m_userName);
	printf("���룺");
	scanf("%s", &m_user.m_userPwd);

	while (!m_login.loginSuccess(m_user.m_userName, m_user.m_userPwd)) {
		printf("����������.\n");
		printf("�û�����");
		scanf("%s", &m_user.m_userName);
		printf("���룺");
		scanf("%s", &m_user.m_userPwd);
	}

	printf("��¼�ɹ���\n");
	printHelp();

	
	fd_set fdread, fedwrite;
	FD_ZERO(&fdread);//��fdread����
	FD_ZERO(&fedwrite);//��fedwrite����

	init();

	send(m_handle, m_user.m_userName, sizeof(m_user.m_userName) - 1, 0);
	while (!m_quit)
	{
		FD_SET(m_handle, &fdread);
		if (writing == 0)
			FD_SET(m_handle, &fedwrite);
		struct timeval timeout = { 1,0 };//ÿ��Select�ȴ�1��
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
			if (FD_ISSET(m_handle, &fdread))//�д����¼�
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
			if (FD_ISSET(m_handle, &fedwrite))//�д�д�¼�
			{
				FD_ZERO(&fedwrite);//��fedwrite����
				writing = 1;//��ʾ����д��
				std::thread sendtask(std::bind(&client::sendata, this));
				sendtask.detach();//�����̺߳������̷����һ��಻Ӱ��
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
	std::cin.getline(sendbuf, 1024);//��ȡһ��

	if (strcmp(sendbuf, "h") == 0)
	{
		printHelp();
	}
	else if (strcmp(sendbuf, "l") == 0) // �鿴���Ѻ�Ⱥ�б�
	{
		m_login.QueryChatList(m_user.m_userName);
	}
	else if (strcmp(sendbuf, "q") == 0)
	{
		m_quit = true;
	}
	else
	{	
		if(messageIsVaild()) // �鿴������Ϣ�ĺϷ���
			send(m_handle, sendbuf, sizeof(sendbuf) - 1, 0);
	}
	writing = 0;	
}

void client::printHelp()
{
	printf("h���鿴����\n");
	printf("lf���鿴�����б�\n");
	printf("lg���鿴Ⱥ���б�\n");
	printf("������ѻ���Ⱥ�����ֽ������죬����c�ر����촰��");
	printf("q���˳��ͻ���\n");
}

bool client::messageIsVaild()
{
	return true;
}