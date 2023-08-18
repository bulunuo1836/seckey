#pragma once
#include "CodecFactory.h"
#include "RespondCodec.h"

class RespondFactory :
	public CodecFactory
{
public:
	RespondFactory(std::string enc);
	RespondFactory(RespondMsg* info);

	Codec* createCodec();

	~RespondFactory();

private:
	//需求类型标志
	bool m_flag;
	std::string m_encStr;
	RespondMsg* m_info;
};

