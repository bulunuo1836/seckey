#pragma once

#include<mysql.h>
#include <iostream>
#include"NodeSecKeyInfo.h"
using namespace std;
class mysqlOP
{
public:
	mysqlOP();
	~mysqlOP();

	// 初始化环境连接数据库
	bool connectDB(string ip,string user, string passwd, string dbName);
	int getKeyID(string severID,string clientID);
	string getKeyStr(string severID, string clientID);
	bool updataKeyID(int keyID,string secKey);
	bool writeSecKey(NodeSecKeyInfo* pNode);
	bool deleteSecKey(string severID, string clientID);
	void closeDB();

private:
	// 获取当前时间, 并格式化为字符串
	string getCurTime();

private:
	MYSQL* m_mysql;

	
};

