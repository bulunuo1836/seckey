#pragma once
#include <string>
#include <openssl/aes.h>
using namespace std;

class OpensslAES
{
public:
	// ��ʹ�� 16byte, 24byte, 32byte ����Կ
	OpensslAES(string key);
	~OpensslAES();
	// ����
	string CBCEncrypt(string text);
	// ����
	string CBCDecrypt(string encStr);

private:
	string Crypto(string data, int crypto);
	void generateIvec(unsigned char* ivec);

private:
	AES_KEY encKey;
	AES_KEY decKey;
	string key;	// ��Կ
};


