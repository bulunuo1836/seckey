#include "RequestFactory.h"

RequestFactory::RequestFactory(std::string enc)
{
	m_flag = false;
	m_encStr = enc;
}

RequestFactory::RequestFactory(RequestMsg* info)
{
	m_flag = true;
	m_info = info;
}

Codec* RequestFactory::createCodec()
{
	Codec* codec = nullptr;
	//判断加密解密类型，构造对应的对象
	if (m_flag)
	{
		codec = new RequestCodec(m_info);
	}
	else
	{
		codec = new RequestCodec(m_encStr);
	}
	return codec;
}

RequestFactory::~RequestFactory()
{
}
