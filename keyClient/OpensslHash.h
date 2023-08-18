#pragma once
/*
	使用需要添加openssl库
	依赖库加libcrypto.lib

	C++预处理器中添加 _CRT_SECURE_NO_WARNINGS
*/
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <iostream>
#include <string.h>
using namespace std;
#ifndef HASHTYPE
#define HASHTYPE


	enum HashType
	{
		HASH_MD5,
		HASH_SHA1,
		HASH_SHA224,
		HASH_SHA256,
		HASH_SHA384,
		HASH_SHA512
	};

#endif 
class OpensslHash
{
public:
	OpensslHash(HashType type);
	~OpensslHash();

	void addData(string data);

	string result();

private:
	// md5
	inline void md5Init() { MD5_Init(&m_md5); }
	inline void md5AddData(const char* data)
	{
		MD5_Update(&m_md5, data, strlen(data));
	}

	string md5Result();

	// sha1
	inline void sha1Init() { SHA1_Init(&m_sha1); }
	inline void sha1AddData(const char* data)
	{
		SHA1_Update(&m_sha1, data, strlen(data));
	}
	string sha1Result();

	// sha224
	inline void sha224Init() { SHA224_Init(&m_sha224); }
	inline void sha224AddData(const char* data)
	{
		SHA224_Update(&m_sha224, data, strlen(data));
	}
	string sha224Result();

	// sha256
	inline void sha256Init() { SHA256_Init(&m_sha256); }
	inline void sha256AddData(const char* data)
	{
		SHA256_Update(&m_sha256, data, strlen(data));
	}
	string sha256Result();

	// sha384
	inline void sha384Init() { SHA384_Init(&m_sha384); }
	inline void sha384AddData(const char* data)
	{
		SHA384_Update(&m_sha384, data, strlen(data));
	}
	string sha384Result();

	// sha512
	inline void sha512Init() { SHA512_Init(&m_sha512); }
	inline void sha512AddData(const char* data)
	{
		SHA512_Update(&m_sha512, data, strlen(data));
	}
	string sha512Result();

private:


	HashType m_type;

	MD5_CTX m_md5;
	SHA_CTX m_sha1;
	SHA256_CTX m_sha224;
	SHA256_CTX m_sha256;
	SHA512_CTX m_sha384;
	SHA512_CTX m_sha512;

};

