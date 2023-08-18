#pragma once
#include<iostream>

//默认超时时间
static const int TIMEOUT = 10000;


class TcpSocket
{
public:
	enum ErrorType { ParamError = 3001, TimeoutError, PeerCloseError, MallocError };
	TcpSocket();
	//有参构造，使用已有的套接字来构造Tcp套接字对象
	TcpSocket(int connectFd);
	~TcpSocket();

	//连接服务器
	int connectToHost(std::string ip, unsigned short port, int timeout = TIMEOUT);

	//发送数据
	int sendMsg(std::string msg, int timeout = TIMEOUT);

	//接收数据
	std::string recvMsg(int timeout = TIMEOUT);

	//断开连接
	void disConnect();

private:
	//设置文件描述符非阻塞
	int setNonBlock(int fd);

	//设置阻塞
	int setBlock(int fd);

	//读超时
	int readTimeout(unsigned int waitSeconds);

	//写超时
	int writeTimeout(unsigned int waitSeconds);

	//连接超时
	int connectTimeout(struct sockaddr_in* addr, unsigned int waitSeconds);

	//从缓冲区中读取N个字符
	int readN(void* buf, int count);

	int writeN(const void* buf, int count);

private:
	//通信套接字
	int m_socket;


};


