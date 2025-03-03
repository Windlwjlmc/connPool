#pragma once
#include <string>
#include <mysql.h>
#include <chrono>
#include <time.h>

/*
实现mysql数据库增删改查操作
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

	//刷新连接时间点
	void setAliveTime(){ aliveIdleTime_ = std::clock(); }

	clock_t getAliveIdleTime(){	return std::clock() - aliveIdleTime_; }//返回值单位是ms

private:
	MYSQL *_conn; //表示和mysql server的一条连接
	clock_t  aliveIdleTime_;
};