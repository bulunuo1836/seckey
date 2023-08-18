#pragma once
#include"Codec.h"
#include"Msg.pb.h"
#include<iostream>
using namespace std;


class RespondCodec :public Codec
{
public:
	RespondCodec();
	//解码
	RespondCodec(string encstr);
	//加密
	RespondCodec(RespondMsg* info);

	//空构造
	void initMessage(string encstr);
	void initMessage(RespondMsg* info);
	//解码
	string encodeMsg();
	void* decodeMsg();

	~RespondCodec();

private:
	string m_encStr;
	RespondMsg m_msg;
};

