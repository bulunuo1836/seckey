#include "clientOP.h"
#include<fstream>
#include<sstream>
#include<json/json.h>
#include"OpensslRSA.h"
#include "OpensslHash.h"
#include"OpensslAES.h"
#include "TcpSocket.h"
#include"RequestFactory.h"
#include"RespondFactory.h"
using namespace Json;

clientOP::clientOP(std::string jsonFilePWD)
{
    //解析json文件，获取初始配置
    std::ifstream jsonFile(jsonFilePWD);
    

    Reader jsonReader;
    Value jsonRoot;
    //获取json对象
    jsonReader.parse(jsonFile, jsonRoot);
    jsonFile.close();


    //取出值进行初始化
    this->m_info.severID = jsonRoot["SeverID"].asString();
    this->m_info.clientID = jsonRoot["ClientID"].asString();
    this->m_info.severIP = jsonRoot["SeverIP"].asString();
    this->m_info.severPort = jsonRoot["SeverPort"].asInt();

    std::ifstream AESKey("AESKey.txt");
    AESKey >> m_AESKey;
    AESKey.close();

    

}

clientOP::~clientOP()
{
}

bool clientOP::keyAgree()
{
    //生成密钥对
    OpensslRSA RSAKey;

    RSAKey.generateKey(1024);

    ifstream pubKey("public.pem");

    stringstream buf;
    buf<<pubKey.rdbuf();
    //std::cout << "RSA pubKey:" << buf.str() << endl;
    //初始化序列化数据--->公钥
    
    RequestMsg reqInfo;
    reqInfo.set_severid(this->m_info.severID);
    reqInfo.set_clientid(this->m_info.clientID);
    
    //请求类型1 -- 密钥协商
    reqInfo.set_cmdtype(1);
    reqInfo.set_data(buf.str());

    
    OpensslHash Sha224Hash(HashType::HASH_SHA224);
    Sha224Hash.addData(buf.str());
    //数据签名
    reqInfo.set_sign(RSAKey.sign(Sha224Hash.result()));

    CodecFactory* codecFactory = new RequestFactory(&reqInfo);

    Codec* data = codecFactory->createCodec();

    //获取序列化数据
    string encdata = data->encodeMsg();


    delete codecFactory;
    delete data;



    //连接服务器，发送请求
    TcpSocket* tcpSock = new TcpSocket();
    int ret = tcpSock->connectToHost(m_info.severIP, m_info.severPort);
    if (ret != 0)
    {
        cout << "connect to host fail..........\n";
        return false;
    }

    cout << "connect sucess...........\n";
    //发送公钥
    tcpSock->sendMsg(encdata);

    //等待 服务器发送对称加密密钥
    string decdata = tcpSock->recvMsg();

    //解析对称加密密钥
    codecFactory = new RespondFactory(decdata);

    data = codecFactory->createCodec();

    RespondMsg* resData = (RespondMsg*)data->decodeMsg();
   
    //是否状态正确
    if (!resData->status())
    {
        cout << "key agree fail......\n";
        
        return false;
    }
    cout << "key agree success......\n";
    //解密密文,获取对称加密密钥
    string AESKey = RSAKey.priKeyDecrypt(resData->data());
    //cout << "AESKey" << AESKey << endl;

    //对称加密密钥存入内存
    ofstream AESFile("AESKey.txt");

    AESFile << AESKey;

    AESFile.close();

    m_AESKey = AESKey;


    delete codecFactory;
    delete data;
    delete tcpSock;
    
    return true;
}

bool clientOP::keyCheck()
{
    if (m_AESKey == "")
    {
        cout << "no Key....\n";
        return false;
    }
    //使用密钥发送一段数据（checkkey）
    RequestMsg req;
    req.set_severid(m_info.severID);
    req.set_clientid(m_info.clientID);
    req.set_cmdtype(2);
    std::string randStr = getRandStr(16);
    req.set_data(randStr);
    OpensslAES AESCheck(m_AESKey);
    req.set_sign(AESCheck.CBCEncrypt(randStr));

    CodecFactory* codecFactory = new RequestFactory(&req);

    Codec* data = codecFactory->createCodec();

    //获取序列化数据
    string encdata = data->encodeMsg();


    delete codecFactory;
    delete data;



    //连接服务器，发送请求
    TcpSocket* tcpSock = new TcpSocket();
    int ret = tcpSock->connectToHost(m_info.severIP, m_info.severPort);
    if (ret != 0)
    {
        cout << "connect to host fail..........\n";
        return false;
    }

    cout << "connect sucess...........\n";
    //发送核验
    tcpSock->sendMsg(encdata);
    //阻塞接受回复数据
    string decdata = tcpSock->recvMsg();
    
    //反序列化
    codecFactory = new RespondFactory(decdata);
    data = codecFactory->createCodec();
    RespondMsg* resData = (RespondMsg*)data->decodeMsg();

    //是否状态正确
    if (!resData->status())
    {
        cout << "key check fail......\n";

        return false;
    }
    cout << "key check success......\n";
    
    delete codecFactory;
    delete data;
    delete tcpSock;
    return true;
}

void clientOP::keyDelete()
{
    //只需删除掉服务器上的key即可，客户端不需要删除
    RequestMsg req;
    req.set_severid(m_info.severID);
    req.set_clientid(m_info.clientID);
    req.set_cmdtype(3);

    CodecFactory* codecFactory = new RequestFactory(&req);

    Codec* data = codecFactory->createCodec();

    //获取序列化数据
    string encdata = data->encodeMsg();


    delete codecFactory;
    delete data;

    //连接服务器，发送请求
    TcpSocket* tcpSock = new TcpSocket();
    int ret = tcpSock->connectToHost(m_info.severIP, m_info.severPort);
    if (ret != 0)
    {
        cout << "connect to host fail..........\n";
        return;
    }

    cout << "connect sucess...........\n";
    //发送核验
    tcpSock->sendMsg(encdata);

    //阻塞接受回复数据
    string decdata = tcpSock->recvMsg();

    //反序列化
    codecFactory = new RespondFactory(decdata);
    data = codecFactory->createCodec();
    RespondMsg* resData = (RespondMsg*)data->decodeMsg();

    //是否状态正确
    if (!resData->status())
    {
        cout << "key delete fail......\n";

        return ;
    }
    cout << "key delete success......\n";

    delete codecFactory;
    delete data;
    delete tcpSock;
}

std::string clientOP::getRandStr(int len)
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
        case 0:	// 1-9
            retStr.append(1, rand() % 9 + '1');
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

