#pragma once
#include"CodecFactory.h"
#include"RequestCodec.h"

//请求信息工厂
class RequestFactory:
	public CodecFactory
{
public:
	RequestFactory(std::string enc);
	RequestFactory(RequestMsg* info);
	Codec* createCodec();
	~RequestFactory();
private:
	//判断需求类型 0 解密 ，1 加密
	bool m_flag;
	std::string m_encStr;
	RequestMsg* m_info;
};

