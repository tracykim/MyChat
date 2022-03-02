#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "client.h"

client::client()
{
	m_handle = 0;
	writing = 0;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);//���ַ�������ת��uint32_t
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
	printf("connect IP:%s  Port:%d succesfully\n", SERVER_IP, SERVER_PORT);//�����ɹ�
}

void client::process()
{
	char recvbuf[1024];
	char name[1024];
	char pwd[1024];

	fd_set fdread, fedwrite;
	FD_ZERO(&fdread);//��fdread����
	FD_ZERO(&fedwrite);//��fedwrite����

	init();

	printf("��ӭ�����ҵ������ң����¼.\n");
	printf("�û�����");
	scanf("%s", &name);
	printf("���룺");
	scanf("%s", &pwd);

	/// ��¼
	while (!m_login.loginSuccess(name, pwd)) {
		printf("����������.\n");
		printf("�û�����");
		scanf("%s", &name);
		printf("���룺");
		scanf("%s", &pwd);
	}
	
	printf("��¼�ɹ���\n");
	printHelp();

	while (1)
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
	//char middle[1024];

	std::cin.getline(sendbuf, 1024);//��ȡһ��

	if (strcmp(sendbuf, "h") == 0)
	{
		printHelp();
	}
	else if (strcmp(sendbuf, "lf") == 0) // �鿴������ϵ��
	{
		m_login.QueryFriendList("James");
	}
	else if (strcmp(sendbuf, "lg") == 0) // �鿴������ϵ��
	{
		m_login.QueryFriendList("James");
	}
	else if (strcmp(sendbuf, "q") == 0)
	{
		/// TODO�˳��ͻ���
	}
	else
	{	
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
