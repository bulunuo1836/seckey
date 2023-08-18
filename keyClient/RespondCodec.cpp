#include "RespondCodec.h"

RespondCodec::RespondCodec()
{
}

RespondCodec::RespondCodec(string encstr)
{
	initMessage(encstr);
}

RespondCodec::RespondCodec(RespondMsg* info)
{
	initMessage(info);
}

void RespondCodec::initMessage(string encstr)
{
	m_encStr = encstr;
}

void RespondCodec::initMessage(RespondMsg* info)
{
	m_msg.set_status(info->status());
	m_msg.set_seckeyid(info->seckeyid());
	m_msg.set_clientid(info->clientid());
	m_msg.set_severid(info->severid());
	m_msg.set_data(info->data());
}

string RespondCodec::encodeMsg()
{
	string out;
	m_msg.SerializeToString(&out);
	return out;
}

void* RespondCodec::decodeMsg()
{
	m_encStr = m_msg.ParseFromString(m_encStr);
	return &m_msg;
}

RespondCodec::~RespondCodec()
{
}
