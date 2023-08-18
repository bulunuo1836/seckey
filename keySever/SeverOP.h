#pragma once

#include<iostream>
#include<map>
#include "Msg.pb.h"
#include"TcpSever.h"
#include"mysqlOP.h"
using namespace std;
class SeverOP
{
public:
	SeverOP(string jsonFilePWD);
	~SeverOP();

	void startSever();

	string keyAgree(RequestMsg* msg);
	string keyCheck(RequestMsg* msg);
	string keyDelete(RequestMsg* msg);
	string resTest(RequestMsg* msg);

public:
	friend void* threadWork(void* arg);
private:
	string getRandStr(int len);
private:
	string m_severID;
	unsigned short m_port;
	string m_DBUser;
	string m_DBPasswd;
	string m_DBName;
	string m_severIP;
	map<pthread_t,TcpSocket*> m_list;
	TcpServer* m_sever;
	mysqlOP* m_sqlOP;
	
};

void* threadWork(void* arg);

