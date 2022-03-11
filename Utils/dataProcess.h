#pragma once
#include <stdio.h>
#include <string>
void processData(char& dataType, char fromName[128], char toName[128], char data[1024], const char* buf);
void splitData(int dataType, const char* fromName, const char* toName, const char* data, char* sendbuf);
