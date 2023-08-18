#pragma once
#include <string>
#include <openssl/aes.h>
using namespace std;

class OpensslAES
{
public:
	// 可使用 16byte, 24byte, 32byte 的秘钥
	OpensslAES(string key);
	~OpensslAES();
	// 加密
	string CBCEncrypt(string text);
	// 解密
	string CBCDecrypt(string encStr);

private:
	string Crypto(string data, int crypto);
	void generateIvec(unsigned char* ivec);

private:
	AES_KEY encKey;
	AES_KEY decKey;
	string key;	// 秘钥
};


