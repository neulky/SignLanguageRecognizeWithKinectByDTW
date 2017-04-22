#include"Client.h"
#include <winsock2.h>

SOCKET sockClient;

void Client::Connect()
{
	WSADATA wsaData;
	SOCKADDR_IN addrServer;//����˵�ַ
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//�½��ͻ���socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//����Ҫ���ӵķ���˵�ַ
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.2"); //Ŀ��IP(127.0.0.2�ǻ��͵�ַ)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//���Ӷ˿�6000
	//���ӵ������
	connect(sockClient, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
}

//��������
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

	const char * message = sendStr.c_str();           //��string����ת����char*
	send(sockClient, message, strlen(message) + 1, 0); //������Ϣ
}

//��������
void Client::recvMessage()
{
	char receiveMessage[50] = {};
	recv(sockClient, receiveMessage, 100, 0);
	printf("%s", receiveMessage);
}

//�ر�socket
void Client::Close()
{
	closesocket(sockClient);
	WSACleanup();
}
