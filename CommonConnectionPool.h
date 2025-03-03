#pragma once
#include <queue>
#include <string>
#include <mutex>
#include <memory>
#include <thread>
#include <atomic>
#include <condition_variable>

#include "Connection.h"

/*
实现连接池功能模块
*/

class ConnectionPool
{
public:
	//获取连接池对象实例
	static ConnectionPool* getConnectionPool();

	std::shared_ptr<Connection> getConnection();
private:
	ConnectionPool(); //单例#1 构造函数私有化
	bool initConnectionPool_();
	void produceNewConnection();
	void destroyIdleConnection();

	int initSize_;
	int maxSize_;

	std::string ip_;
	int port_;
	std::string userName_;
	std::string password_;
	std::string dbname_;

	int maxIdleTime_; //最大空闲
	int timeout_;	//超时连接
	

	std::queue<Connection*> conns_;
	std::mutex queueMtx_;
	std::atomic_int connSize_;
	std::condition_variable va_;
};