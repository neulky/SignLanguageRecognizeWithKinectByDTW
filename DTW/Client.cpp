#include"Client.h"
#include <winsock2.h>

SOCKET sockClient;

void Client::Connect()
{
	WSADATA wsaData;
	SOCKADDR_IN addrServer;//服务端地址
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//新建客户端socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//定义要连接的服务端地址
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.2"); //目标IP(127.0.0.2是回送地址)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//连接端口6000
	//连接到服务端
	connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
}

//发送数据
void Client::sendMessage(point sequence[], int count)
{
	string sendStr;
	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < POINT_NUM_EACH_FRAME;j++)
		switch (j)
		{
		case 0: sendStr += to_string(sequence[i].x); sendStr += ','; break;
		case 1: sendStr += to_string(sequence[i].y); break;
		default: break;
		}
		sendStr += '@';
	}

	const char * message = sendStr.c_str();           //将string类型转换成char*
	send(sockClient, message, strlen(message) + 1, 0); //发送消息
}

//接收数据
void Client::recvMessage()
{
	char receiveMessage[50] = {};
	recv(sockClient, receiveMessage, 100, 0);
	printf("%s", receiveMessage);
}

//关闭socket
void Client::Close()
{
	closesocket(sockClient);
	WSACleanup();
}
