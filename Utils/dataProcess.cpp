#define _CRT_SECURE_NO_WARNINGS
#include "dataProcess.h"
void processData(char& dataType, char fromName[128], char toName[128], char data[1024], const char* buf)
{
	dataType = buf[0];
	int i = 2, p = 0, q = 0, r = 0;
	// 野割From
	while (buf[i] != '\0' && buf[i] != ' ')
	{
		fromName[p++] = buf[i++];
	}
	fromName[p] = '\0';
	i++;

	// 野割To
	while (buf[i] != '\0' && buf[i] != ' ')
	{
		toName[q++] = buf[i++];
	}
	toName[q] = '\0';
	i++;

	// 野割data
	while (buf[i] != '\0')
	{
		data[r++] = buf[i++];
	}
	data[r] = '\0';
}

void splitData(int dataType, const char* fromName, const char* toName, const char* data, char* sendbuf)
{
	sprintf(sendbuf, "%d ", dataType);
	strcat(sendbuf, fromName);
	strcat(sendbuf, " ");
	strcat(sendbuf, toName);
	strcat(sendbuf, " ");
	strcat(sendbuf, data);
}