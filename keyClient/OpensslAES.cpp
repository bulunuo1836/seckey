#include "OpensslAES.h"


OpensslAES::OpensslAES(string key)
{
	if (key.size() == 16 || key.size() == 24 || key.size() == 32)
	{
		const unsigned char* aesKey = (const unsigned char*)key.data();
		AES_set_encrypt_key(aesKey, key.size() * 8, &encKey);
		AES_set_decrypt_key(aesKey, key.size() * 8, &decKey);
		this->key = key;
	}
}

OpensslAES::~OpensslAES()
{
}

string OpensslAES::CBCEncrypt(string text)
{
	return Crypto(text, AES_ENCRYPT);
}

string OpensslAES::CBCDecrypt(string encStr)
{
	return Crypto(encStr, AES_DECRYPT);
}

string OpensslAES::Crypto(string data, int crypto)
{
	AES_KEY* key = crypto == AES_ENCRYPT ? &encKey : &decKey;

	unsigned char ivec[AES_BLOCK_SIZE];
	int length = data.size() + 1;	
	if (length % 16)
	{
		length = (length / 16 + 1) * 16;
	}
	char* out = new char[length];
	generateIvec(ivec);

	//cbcº”Ω‚√‹
	AES_cbc_encrypt((const unsigned char*)data.c_str(),
		(unsigned char*)out, length, key, ivec, crypto);

	string retStr = string(out);
	delete[]out;
	return string(retStr);
}

void OpensslAES::generateIvec(unsigned char* ivec)
{
	for (int i = 0; i < AES_BLOCK_SIZE; ++i)
	{
		ivec[i] = key.at(AES_BLOCK_SIZE - i - 1);
	}
}
