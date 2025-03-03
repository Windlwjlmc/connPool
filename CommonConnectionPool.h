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
ʵ�����ӳع���ģ��
*/

class ConnectionPool
{
public:
	//��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();

	std::shared_ptr<Connection> getConnection();
private:
	ConnectionPool(); //����#1 ���캯��˽�л�
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

	int maxIdleTime_; //������
	int timeout_;	//��ʱ����
	

	std::queue<Connection*> conns_;
	std::mutex queueMtx_;
	std::atomic_int connSize_;
	std::condition_variable va_;
};