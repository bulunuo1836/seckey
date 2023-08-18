#pragma once
#include<iostream>

struct clientInfo
{
	std::string severID;
	std::string clientID;
	std::string severIP;
	unsigned short severPort;
};

class clientOP
{
public:
	clientOP(std::string jsonFilePWD);
	~clientOP();

	//密钥协商
	bool keyAgree();

	//密钥校验
	bool keyCheck();
	
	//密钥注销
	void keyDelete();

private:
	std::string getRandStr(int len);

private:
	clientInfo m_info;
	std::string m_AESKey;
};

