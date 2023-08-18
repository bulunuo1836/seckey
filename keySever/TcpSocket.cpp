#include "TcpSocket.h"

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cstdlib>

TcpSocket::TcpSocket()
{
}

TcpSocket::TcpSocket(int connectFd)
{
	this->m_socket = connectFd;
}

TcpSocket::~TcpSocket()
{
}

int TcpSocket::connectToHost(std::string ip, unsigned short port, int timeout)
{
	int ret = 0;

	//��������
	if (port <= 0 || port > 65535 || timeout < 0)
	{
		ret = ParamError;
		return ret;
	}

	//�����׽���
	this->m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->m_socket < 0)
	{
		ret = errno;
		printf("func socket() err: %d\n", ret);
		return ret;
	}

	//����
	struct sockaddr_in severAddr {};;
	severAddr.sin_family = AF_INET;
	severAddr.sin_port = htons(port);
	severAddr.sin_addr.s_addr = inet_addr(ip.data());

	ret = connectTimeout((struct sockaddr_in*)&severAddr, (unsigned int)timeout);

	if (ret < 0)
	{
		//��ʱ
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = TimeoutError;
			return ret;
		}
		//�����쳣
		else
		{
			printf("func connectTimeout() err: %d", errno);
			return errno;
		}
	}
	return ret;
}

int TcpSocket::sendMsg(std::string msg, int timeout)
{
	//��ʱ���
	int ret = writeTimeout(timeout);
	if (ret == 0)
	{
		int writed = 0;
		int dataLen = msg.size() + 4;
		unsigned char* netData = (unsigned char*)malloc(dataLen);
		if (netData == NULL)
		{
			ret = MallocError;
			printf("func sendMsg() malloc err:%d", ret);
			return ret;
		}

		//ת�������ֽ���
		int netLen = htonl(msg.size());
		memcpy(netData, &netLen, 4);
		memcpy(netData + 4, msg.data(), msg.size());

		//��������
		writed = writeN(netData, dataLen);

		//����ʧ��,�ͷ��ڴ淵��
		if (writed < dataLen)
		{
			if (netData != NULL)
			{
				free(netData);
				netData = NULL;
			}
			return writed;
		}

		if (netData != NULL)
		{
			free(netData);
			netData = NULL;
		}

	}
	else
	{
		if (ret == -1 && errno == ETIMEDOUT)
		{
			ret = TimeoutError;
			printf("func sendMsg() timeout err:%d\n", ret);
		}

	}
	return ret;
}

std::string TcpSocket::recvMsg(int timeout)
{
	//����ʱ���
	int ret = readTimeout(timeout);
	if (ret != 0)
	{
		if (ret == -1 || errno == ETIMEDOUT)
		{
			printf("readTimeout() err: TimeoutError\n");
			return std::string();
		}
		else
		{
			printf("readTimeout() err:%d\n", ret);
			return std::string();
		}
	}

	int netdataLen = 0;
	//��ȡ���ݳ���
	ret = readN(&netdataLen, 4);
	if (ret == -1)
	{
		printf("func readN() err:%d\n", ret);
		return std::string();
	}
	else if (ret < 4)
	{
		printf("func readN() err peer closed:%d\n", ret);
		return std::string();
	}

	int n = ntohl(netdataLen);

	char* buf = (char*)malloc(n + 1);

	if (buf == NULL)
	{
		ret = MallocError;
		printf("func recvMsg() malloc err\n");
		return NULL;
	}

	//��ȡ��������
	ret = readN(buf, n);
	if (ret == -1)
	{
		printf("func readn() err:%d \n", ret);
		return std::string();
	}
	else if (ret < n)
	{
		printf("func readn() err peer closed:%d \n", ret);
		return std::string();
	}

	buf[n] = '\0';

	std::string data = std::string(buf);

	free(buf);

	return data;
}

void TcpSocket::disConnect()
{
	if (this->m_socket >= 0)
	{
		close(this->m_socket);
	}
}





int TcpSocket::setNonBlock(int fd)
{
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		return flags;
	}

	flags |= O_NONBLOCK;

	int ret = fcntl(fd, F_SETFL, flags);
	return ret;

}

int TcpSocket::setBlock(int fd)
{
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
	{
		return flags;
	}

	flags &= ~O_NONBLOCK;

	int ret = fcntl(fd, F_SETFL, flags);
	return ret;
}
/*
	����ʱ���

*/
int TcpSocket::readTimeout(unsigned int waitSeconds)
{
	int ret = 0;
	if (waitSeconds > 0)
	{
		fd_set writeFdSet{};
		struct timeval timeout;
		timeout.tv_sec = waitSeconds;
		timeout.tv_usec = 0;

		FD_ZERO(&writeFdSet);
		FD_SET(this->m_socket, &writeFdSet);

		do
		{
			ret = select(this->m_socket + 1, NULL, &writeFdSet, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		/*
			select����ֵ�Ķ������
			select<0����
			select = 0��ʱ
			select > 0�����ж��¼�����

		*/
		//��ʱ
		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;

		}
		else if (ret == 1)
		{
			ret = 0;
		}
	}
	return ret;
}
/*
	д��ʱ���

	�ɹ�����0
	ʧ�ܷ���-1����ʱerrno = ETIMEDOUT
*/
int TcpSocket::writeTimeout(unsigned int waitSeconds)
{
	int ret = 0;
	if (waitSeconds > 0)
	{
		fd_set writeFdSet{};
		struct timeval timeout;
		timeout.tv_sec = waitSeconds;
		timeout.tv_usec = 0;

		FD_ZERO(&writeFdSet);
		FD_SET(this->m_socket, &writeFdSet);

		do
		{
			ret = select(this->m_socket + 1, NULL, &writeFdSet, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		//��ʱ
		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;

		}
		else
		{
			ret = 0;
		}
	}
	return ret;
}
/*
	���ӷ�����

	�ɹ�����0
	ʧ�ܷ���-1������ʱʧ��errno = ETIMEDOUT
*/
int TcpSocket::connectTimeout(sockaddr_in* addr, unsigned int waitSeconds)
{
	int ret;

	socklen_t addrLen = sizeof(struct sockaddr_in);

	if (waitSeconds > 0)
	{
		//���÷�����IO
		setNonBlock(this->m_socket);
	}

	//����
	ret = connect(this->m_socket, (struct sockaddr*)addr, addrLen);

	//������ģʽ������connect����-1��errno = EINPROGRESS�������ӽ�����
	if (ret < 0 && errno == EINPROGRESS)
	{

		fd_set connectFdSet{};
		struct timeval timeout {};;
		FD_ZERO(&connectFdSet);
		FD_SET(m_socket, &connectFdSet);
		timeout.tv_sec = waitSeconds;
		timeout.tv_usec = 0;

		//�������ӽ������
		do
		{
			ret = select(m_socket + 1, NULL, &connectFdSet, NULL, &timeout);
		} while (ret < 0 && errno == EINTR);

		//��ʱ
		if (ret == 0)
		{
			ret = -1;
			errno = ETIMEDOUT;
		}
		//��������
		else if (ret < 0)
		{
			return -1;
		}
		//�׽��ֿ�д(1.���ӳɹ��� 2.�׽��ִ���)
		else if (ret == 1)
		{
			int err = 0;
			socklen_t sockLen = sizeof(err);
			int sockoptRet = getsockopt(this->m_socket, SOL_SOCKET, SO_ERROR, &err, &sockLen);

			if (sockoptRet == -1)
			{
				return -1;
			}
			//�ɹ���������
			if (err == 0)
			{
				ret = 0;
			}
			else
			{
				errno = err;
				ret = -1;
			}

		}

	}
	if (waitSeconds > 0)
	{
		setBlock(this->m_socket);
	}
	return ret;
}

int TcpSocket::readN(void* buf, int count)
{
	size_t nLeft = count;
	ssize_t nRead;

	char* bufp = (char*)buf;

	while (nLeft > 0)
	{
		if ((nRead = read(this->m_socket, bufp, nLeft)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		else if (nRead == 0)
		{
			return count - nLeft;
		}
		bufp += nRead;
		nLeft -= nRead;
	}
	return count;
}

int TcpSocket::writeN(const void* buf, int count)
{
	size_t nLeft = count;
	ssize_t nWrite = 0;

	char* bufp = (char*)buf;

	while (nLeft > 0)
	{
		if ((nWrite = write(this->m_socket, bufp, nLeft)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			return -1;
		}
		else if (nWrite == 0)
		{
			continue;
		}
		bufp += nWrite;
		nLeft -= nWrite;
	}
	return count;
}

