#include "RequestCodec.h"

RequestCodec::RequestCodec()
{
}

RequestCodec::RequestCodec(string encstr)
{
	initMessage(encstr);
}

RequestCodec::RequestCodec(RequestMsg* info)
{
	initMessage(info);
}

void RequestCodec::initMessage(string encstr)
{
	m_encStr = encstr;
}

void RequestCodec::initMessage(RequestMsg* info)
{
	m_msg.set_cmdtype(info->cmdtype());
	m_msg.set_clientid(info->clientid());
	m_msg.set_severid(info->severid());
	m_msg.set_sign(info->sign());
	m_msg.set_data(info->data());

}

string RequestCodec::encodeMsg()
{
	string out;
	m_msg.SerializeToString(&out);
	return out;
}

void* RequestCodec::decodeMsg()
{
	m_msg.ParseFromString(m_encStr);
	return &m_msg;
}

RequestCodec::~RequestCodec()
{
}
