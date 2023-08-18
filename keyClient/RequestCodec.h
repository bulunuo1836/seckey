#pragma once

#include"Msg.pb.h"
#include"Codec.h"
#include<iostream>
using namespace std;


class RequestCodec:public Codec
{
public:
	RequestCodec();
	//解码
	RequestCodec(string encstr);
	//加密
	RequestCodec(RequestMsg* info);

	//空构造
	void initMessage(string encstr);
	void initMessage(RequestMsg* info);
	//解码
	string encodeMsg();
	void* decodeMsg();

	~RequestCodec();

private:
	string m_encStr;
	RequestMsg m_msg;
};

