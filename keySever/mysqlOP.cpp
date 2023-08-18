#include "mysqlOP.h"

mysqlOP::mysqlOP()
{
    this->m_mysql = new MYSQL();
}

mysqlOP::~mysqlOP()
{
}

bool mysqlOP::connectDB(string ip,string user, string passwd, string dbName)
{
    mysql_init(this->m_mysql);

    if (mysql_real_connect(m_mysql, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), 3306, NULL, CLIENT_FOUND_ROWS))
    {
        cout << "connect to DB "<<dbName<<" success\n";
        return true;
    }
    cout << "connect to DB " << dbName << " failed\n";
    return false;
}

int mysqlOP::getKeyID(string severID, string clientID)
{
    int ret = -1;
    string sqlStr = "select keyid from keystore where";
    sqlStr += " severid = '"+severID + "' and clientid = '" + clientID + "';";
    if (mysql_query(m_mysql, sqlStr.data()) != 0)
    {
        ret = -2;
        cout << mysql_error(m_mysql)<<endl;
        
    }
    else
    {
        MYSQL_RES* res = mysql_store_result(m_mysql);
        MYSQL_ROW row = mysql_fetch_row(res);

        while (row != NULL)
        {
            ret = stoi(row[0]);
            
            row = mysql_fetch_row(res);
        }
    }
    
    return ret;
}

string mysqlOP::getKeyStr(string severID, string clientID)
{
    string temstr = "select keyStr from  keystore where severid = '";
    temstr += severID + "' and clientid = '" + clientID + "';";
    //cout << "update :" << temstr << endl;
    string retStr = "";
    int ret = mysql_query(m_mysql, temstr.data());
    if (!ret)
    {
        MYSQL_RES* res = mysql_store_result(m_mysql);
        MYSQL_ROW row = mysql_fetch_row(res);
        while (row != NULL)
        {
            retStr = row[0];

            row = mysql_fetch_row(res);
        }
    }
    else
    {
        std::cout << mysql_error(m_mysql);
       
    }
    return retStr;

    
}

bool mysqlOP::updataKeyID(int keyID, string secKey)
{
    string temstr = "update keystore set keyStr = '";
    temstr += secKey + "' where keyid = " + to_string(keyID)+";";
    //cout << "update :" << temstr << endl;
    int ret = mysql_query(m_mysql, temstr.data());
    if(!ret)
    {
        mysql_commit(m_mysql);
        return true;
    }
    else
    {
        std::cout << mysql_error(m_mysql);
        return false;
    }
}

bool mysqlOP::writeSecKey(NodeSecKeyInfo* pNode)
{
   
    string temstr = "insert into keystore(severid,clientid,keyStr,status) value(";
    
    temstr +="'"+ string(pNode->serverID) + "','" + string(pNode->clientID) + "','" + string(pNode->seckey) + "'," + to_string(pNode->status) + ");";
    //std::cout << temstr << std::endl;
    int ret = mysql_query(m_mysql, temstr.data());
    
    if (!ret)
    {
        mysql_commit(m_mysql);
        return true;
    }
    else
    {
        std::cout<<mysql_error(m_mysql);
        return false;
    }
}

bool mysqlOP::deleteSecKey(string severID, string clientID)
{
    string temstr = "delete from keystore where severid = '";
    temstr += severID + "' and clientid = '" + clientID + "';";
    //cout << "update :" << temstr << endl;
    int ret = mysql_query(m_mysql, temstr.data());
    if (!ret)
    {
        mysql_commit(m_mysql);
        return true;
    }
    else
    {
        std::cout << mysql_error(m_mysql);
        return false;
    }
}

void mysqlOP::closeDB()
{
    mysql_close(m_mysql);
}

string mysqlOP::getCurTime()
{
    return string();
}
