#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "client.h"

client::client()
{
	user = 0;
	writing = 0;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = SERVER_PORT;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);//���ַ�������ת��uint32_t
}

void client::init()
{
	WSADATA   wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	user = socket(AF_INET, SOCK_STREAM, 0);//����ipv4,TCP���䣬�ɹ�ʱ���طǸ���socket������

	if (user <= 0)
	{
		perror("establish client faild");
		printf("Error at socket(): %ld\n", WSAGetLastError());
		exit(1);
	};
	printf("establish succesfully\n");//�����ɹ�������ʽ�ĵȴ�����������
	if (connect(user, (const sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
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
	fd_set fdread, fedwrite;
	FD_ZERO(&fdread);//��fdread����
	FD_ZERO(&fedwrite);//��fedwrite����

	init();

	while (1)
	{
		FD_SET(user, &fdread);
		if (writing == 0)
			FD_SET(user, &fedwrite);
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
			if (FD_ISSET(user, &fdread))//�д����¼�
			{
				int size = recv(user, recvbuf, sizeof(recvbuf) - 1, 0);
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
			if (FD_ISSET(user, &fedwrite))//�д�д�¼�
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
	send(user, sendbuf, sizeof(sendbuf) - 1, 0);
	writing = 0;
}