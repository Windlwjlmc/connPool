#pragma once
#include <string>
#include <mysql.h>
#include <chrono>
#include <time.h>

/*
ʵ��mysql���ݿ���ɾ�Ĳ����
*/

class Connection
{
public:
	Connection();
	~Connection();
	bool connect(std::string ip,
		unsigned short port,
		std::string username,
		std::string password,
		std::string dbname);
	
	bool update(std::string sql);
	MYSQL_RES* query(std::string sql);

	//ˢ������ʱ���
	void setAliveTime(){ aliveIdleTime_ = std::clock(); }

	clock_t getAliveIdleTime(){	return std::clock() - aliveIdleTime_; }//����ֵ��λ��ms

private:
	MYSQL *_conn; //��ʾ��mysql server��һ������
	clock_t  aliveIdleTime_;
};