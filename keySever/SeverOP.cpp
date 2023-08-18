#include "SeverOP.h"
#include<fstream>
#include<json/json.h>
#include<unistd.h>
#include "OpensslRSA.h"
#include "OpensslHash.h"
#include "RespondCodec.h"
#include "RespondFactory.h"
#include"RequestFactory.h"
#include "OpensslAES.h"
using namespace Json;
SeverOP::SeverOP(string jsonFilePWD)
{
	//读取json文件---初始化配置
	ifstream jsonFile(jsonFilePWD);

	Reader jsonReader;
	Value jsonRoot;
	jsonReader.parse(jsonFile, jsonRoot);

	m_severID = jsonRoot["SeverID"].asString();
	m_port = jsonRoot["SeverPort"].asInt();
	m_DBName = jsonRoot["DBName"].asString();
	m_DBPasswd = jsonRoot["DBPasswd"].asString();
	m_DBUser = jsonRoot["DBUser"].asString();
	m_severIP = jsonRoot["SeverIP"].asString();

	//连接数据库
	m_sqlOP = new mysqlOP();

	m_sqlOP->connectDB(m_severIP,m_DBUser,m_DBPasswd,m_DBName);

}

SeverOP::~SeverOP()
{
	if (m_sever != NULL)
	{
		delete m_sever;
	}
	if (m_sqlOP != NULL)
	{
		m_sqlOP->closeDB();
		delete m_sqlOP;
	}
}

void SeverOP::startSever()
{

	m_sever = new TcpServer();
	m_sever->setListen(m_port);
	
	while (1)
	{
		cout << "waiting connect ...........\n";
		TcpSocket* tcp = m_sever->acceptConn();
		if (tcp == NULL)
		{
			continue;
		}
		cout << "client connect sucess......\n";

		pthread_t tid;
		pthread_create(&tid, NULL, threadWork, this);
		m_list.insert(make_pair(tid, tcp));
	}

}

string SeverOP::keyAgree(RequestMsg* msg)
{
	RespondMsg info;
	//保存公钥
	ofstream pubKey("public.pem");
	cout << "reqMsg:" << msg->data()<<endl;
	pubKey << msg->data();
	pubKey.close();


	//检验签名是否正确
	OpensslRSA RSAKey("public.pem",false);
	OpensslHash sha224Hash(HashType::HASH_SHA224);

	sha224Hash.addData(msg->data());
	//检测数据是否被修改
	if ( !(RSAKey.verify(sha224Hash.result(), msg->sign())) )
	{
		info.set_status(false);
		cout << "sign verify fail..........\n";
	}
	else
	{
		cout << "sign verify success..........\n";
		string randStr = getRandStr(16);

		//cout << "AESKey:" << randStr << endl;
		//公钥加密
		string secStr = RSAKey.pubKeyEncrypt(randStr);

		//回复数据整理
		info.set_clientid(msg->clientid());
		info.set_severid(msg->severid());
		info.set_seckeyid(1);
		info.set_status(true);
		info.set_data(secStr);
		
		//对称加密密钥写入数据库
		NodeSecKeyInfo* AESKeyInfo = new NodeSecKeyInfo();
		strncpy(AESKeyInfo->clientID, msg->clientid().c_str(), sizeof(msg->clientid().c_str()));
		strncpy(AESKeyInfo->serverID, msg->severid().c_str(), sizeof(msg->severid().c_str()));
		strncpy(AESKeyInfo->seckey, randStr.c_str(), sizeof(randStr.c_str()));
		
		//AESKeyInfo->serverID = msg->severid();
		AESKeyInfo->status = 1;
		//AESKeyInfo->seckey = randStr;

		//查看密钥情况，是否存在，存在更新，不存在写入
		int ret = m_sqlOP->getKeyID(AESKeyInfo->serverID, AESKeyInfo->clientID);
		if (ret == -1)
		{
			//写入数据库失败
			if (!m_sqlOP->writeSecKey(AESKeyInfo))
			{
				//密钥协商失败
				info.set_status(false);
				cout << "seckey write failed......\n";
			}
			else
			{
				cout << "seckey write success......\n";
			}
		}
		//sql异常，不写入，不更新
		else if (ret == -2)
		{
			info.set_status(false);

		}
		else
		{
			if (!m_sqlOP->updataKeyID(ret,randStr))
			{
				info.set_status(false);
				cout << "seckey update failed......\n";
			}
			else
			{
				cout << "seckey update success......\n";
			}
		}
		
		
		if (AESKeyInfo != NULL)
		{
			delete AESKeyInfo;
		}
	}


	CodecFactory* factory = new RespondFactory(&info);
	Codec* c = factory->createCodec();
	string data = c->encodeMsg();

	delete factory;
	delete c;
	// 发送数据
	return data;

}

//密钥核对
string SeverOP::keyCheck(RequestMsg* msg)
{
	RespondMsg info;
	//获取对应客户端密钥
	string AESKey = m_sqlOP->getKeyStr(msg->severid(), msg->clientid());
	//没找到对应密钥
	if (AESKey == "")
	{
		cout << "not found key in DB\n";
		info.set_status(false);
		
	}
	else
	{
		//密钥核对
		OpensslAES AESCheck(AESKey);

		string decData = AESCheck.CBCDecrypt(msg->sign());
		
		if (decData != msg->data())
		{
			cout << "keyCheck failed.....\n";
			info.set_status(false);
		}
		else
		{
			cout << "keyCheck success.......\n";
			info.set_status(true);
		}
		

	}
	info.set_clientid(msg->clientid());
	info.set_severid(msg->severid());
	info.set_seckeyid(m_sqlOP->getKeyID(msg->severid(),msg->clientid()));
	//回复数据序列化
	CodecFactory* factory = new RespondFactory(&info);
	Codec* c = factory->createCodec();
	string data = c->encodeMsg();

	delete factory;
	delete c;
	// 发送数据
	return data;
	
}

string SeverOP::keyDelete(RequestMsg* msg)
{
	RespondMsg res;
	if (m_sqlOP->deleteSecKey(msg->severid(), msg->clientid()))
	{
		cout << "delete secKey success\n";
		res.set_status(true);
	}
	else
	{
		cout << "delete secKey failed\n";
		res.set_status(false);
	}
	res.set_severid(msg->severid());
	res.set_clientid(msg->clientid());
	
	//回复数据序列化
	CodecFactory* factory = new RespondFactory(&res);
	Codec* c = factory->createCodec();
	string data = c->encodeMsg();

	delete factory;
	delete c;
	// 发送数据
	return data;
}

string SeverOP::resTest(RequestMsg* msg)
{
	return string();
}

string SeverOP::getRandStr(int len)
{
	// 以当前时间为种子
	srand(time(NULL));	
	string retStr = string();
	char* buf = "~`@#$%^&*()_+=-{}[];':";
	for (int i = 0; i < len; ++i)
	{
		int flag = rand() % 4;
		switch (flag)
		{
		case 0:	// 0-9
			retStr.append(1, rand() % 10 + '0');
			break;
		case 1:	// a-z
			retStr.append(1, rand() % 26 + 'a');
			break;
		case 2:	// A-Z
			retStr.append(1, rand() % 26 + 'A');
			break;
		case 3:	// 特殊字符
			retStr.append(1, buf[rand() % strlen(buf)]);
			break;
		}
	}
	return retStr;
	
}

void* threadWork(void* arg)
{
	sleep(1);
	SeverOP* op = (SeverOP*)arg;
	// 1. 接收客户端数据 -> 编码
	TcpSocket* tcp = op->m_list[pthread_self()];
	string msg = tcp->recvMsg();
	
	// 2. 反序列化 -> 得到原始数据 RequestMsg 类型
	CodecFactory* factory = new RequestFactory(msg);
	Codec* c = factory->createCodec();
	RequestMsg* req = (RequestMsg*)c->decodeMsg();

	// 3. 取出数据
	// 判断客户端是什么请求
	string data;
	switch (req->cmdtype())
	{
	case 1:
		// 秘钥协商
		cout << "keyAgree\n";
		data = op->keyAgree(req);
		break;
	case 2:
		// 秘钥校验
		cout << "keyCheck\n";
		data = op->keyCheck(req);
		break;
	case 3:
		//密钥删除
		cout << "keyDelete\n";
		data = op->keyDelete(req);
		break;
	case 4:
		//数据回复测试
		data = op->resTest(req);
	default:
		break;
	}
	delete factory;
	delete c;
	
	tcp->sendMsg(data);
	tcp->disConnect();
	op->m_list.erase(pthread_self());
	delete tcp;

	return NULL;
}
