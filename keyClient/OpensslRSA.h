#pragma once

/*
	使用需要添加openssl库
	依赖库加libcrypto.lib

	C++预处理器中添加 _CRT_SECURE_NO_WARNINGS
*/

#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace std;


enum RSAHashType
{
	T_MD5 = NID_md5,
	T_SHA1 = NID_sha1,
	T_SHA224 = NID_sha224,
	T_SHA256 = NID_sha256,
	T_SHA384 = NID_sha384,
	T_SHA512 = NID_sha512
};

class OpensslRSA
{
public:
	OpensslRSA();
	OpensslRSA(string fileName, bool isPrivate = true);
	~OpensslRSA();

	// 生成RSA密钥对
	void generateKey(int bits, string pub = "public.pem", string pri = "private.pem");
	// 公钥加密
	string pubKeyEncrypt(string data);
	// 私钥解密
	string priKeyDecrypt(string encData);
	// 使用RSA签名
	string sign(string data, RSAHashType type = T_SHA224);
	// 使用RSA验证签名
	bool verify(string data, string signData, RSAHashType type = T_SHA224);

private:
	// 得到公钥
	bool initPublicKey(string pubfile);
	// 得到私钥
	bool initPrivateKey(string prifile);

	string toBase64(const char* str, int len);

	char* fromBase64(string str);

private:

	RSA* publicKey;	// 私钥
	RSA* privateKey;	// 公钥
};

