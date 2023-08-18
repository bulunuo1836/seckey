#pragma once
#include<iostream>

//Ĭ�ϳ�ʱʱ��
static const int TIMEOUT = 10000;


class TcpSocket
{
public:
	enum ErrorType { ParamError = 3001, TimeoutError, PeerCloseError, MallocError };
	TcpSocket();
	//�вι��죬ʹ�����е��׽���������Tcp�׽��ֶ���
	TcpSocket(int connectFd);
	~TcpSocket();

	//���ӷ�����
	int connectToHost(std::string ip, unsigned short port, int timeout = TIMEOUT);

	//��������
	int sendMsg(std::string msg, int timeout = TIMEOUT);

	//��������
	std::string recvMsg(int timeout = TIMEOUT);

	//�Ͽ�����
	void disConnect();

private:
	//�����ļ�������������
	int setNonBlock(int fd);

	//��������
	int setBlock(int fd);

	//����ʱ
	int readTimeout(unsigned int waitSeconds);

	//д��ʱ
	int writeTimeout(unsigned int waitSeconds);

	//���ӳ�ʱ
	int connectTimeout(struct sockaddr_in* addr, unsigned int waitSeconds);

	//�ӻ������ж�ȡN���ַ�
	int readN(void* buf, int count);

	int writeN(const void* buf, int count);

private:
	//ͨ���׽���
	int m_socket;


};


