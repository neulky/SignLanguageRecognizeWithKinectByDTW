#pragma once
#ifndef CLIENT_H
#define CLIENT_H


#include<stdio.h>
#pragma comment(lib,"ws2_32.lib")
#include"DTW.h"


class Client
{

public:
	void Connect();
	void sendMessage(point sequence[], int count);
	string recvMessage();
	void Close();
};


#endif // !CLIENT_H
