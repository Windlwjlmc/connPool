#include "CommonConnectionPool.h"
#include <fstream>
#include "public.h"
#include <functional>

//线程安全的懒汉单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool; //自动锁
	return &pool;
}

ConnectionPool::ConnectionPool()
{
	if (!initConnectionPool_())
		return;

	for (int i = 0; i < initSize_; ++i)
	{
		Connection* conn = new Connection();
		conn->connect(ip_, port_, userName_, password_, dbname_);
		conn->setAliveTime();
		conns_.push(conn);
		connSize_++;
	}
	
	std::thread productor(std::bind(&ConnectionPool::produceNewConnection,this));
	productor.detach();

	std::thread destroyer(std::bind(&ConnectionPool::destroyIdleConnection, this));
	destroyer.detach();
}

bool ConnectionPool::initConnectionPool_()
{
	FILE* pf = fopen("initconf.ini", "r");
	if (pf == nullptr)
	{
		LOG("can't open config file");
		return false;
	}
		
	
	while (!feof(pf))
	{
		/*initSize = 10
			MaxSize = 1000
			ip = 127.0.0.1
			port = 3306
			userName = root
			password = 123456
			#ms
			timeout = 100
			#s
			maxIdleTime = 60*/
		char buffer[1024] = { 0 };
		fgets(buffer, 1024, pf);
		std::string str = buffer;
		int idx = str.find('=');
		int endIdx = str.find('\n');

		std::string key = str.substr(0, idx);
		std::string value = str.substr(idx + 1, endIdx - idx - 1);

		if (key == "initSize")
		{
			initSize_ = atoi(value.c_str());
		}
		if (key == "MaxSize")
		{
			maxSize_ = atoi(value.c_str());
		}
		else if (key == "ip")
		{
			ip_ = value;
		}
		else if (key == "port")
		{
			port_ = atoi(value.c_str());
		}
		else if (key == "userName")
		{
			userName_ = value;
		}
		else if (key == "password")
		{
			password_ = value;
		}
		else if (key == "timeout")
		{
			timeout_ = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			maxIdleTime_ = atoi(value.c_str());
		}
		else if (key == "dbname")
		{
			dbname_ = value;
		}
	}
	return true;
}

std::shared_ptr<Connection> ConnectionPool::getConnection()
{
	std::unique_lock<std::mutex> lock(queueMtx_);
	while (conns_.empty())
	{	
		if (std::cv_status::timeout == va_.wait_for(lock, std::chrono::milliseconds(timeout_)))
		{
			if (conns_.empty())
			{
				LOG("can't get connection now!");
				return nullptr;
			}
		}
	}

	std::shared_ptr<Connection> conn(conns_.front(),
		[&](Connection* conn) {
			std::unique_lock<std::mutex> lock(queueMtx_);
			conn->setAliveTime();
			conns_.push(conn);
		});
	conns_.pop();
	va_.notify_all();//消费完后通知生产者
	return conn;
}

void ConnectionPool::produceNewConnection()
{
	for(;;)
	{
		std::unique_lock<std::mutex> lock(queueMtx_);
		while (!conns_.empty())
		{
			va_.wait(lock);
		}
		if (connSize_ >= maxSize_)
		{
			LOG("can't create more connections.");
		}
		else
		{
			Connection* conn = new Connection();
			conn->connect(ip_, port_, userName_, password_, dbname_);
			conn->setAliveTime();
			conns_.push(conn);
			connSize_++;

			//通知消费
			va_.notify_all();
		}
	}
}

void ConnectionPool::destroyIdleConnection()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(maxIdleTime_)); //模拟定时效果
		std::unique_lock<std::mutex> lock(queueMtx_);
		va_.wait(lock);
		while (connSize_ > initSize_)
		{
			Connection* conn = conns_.front();
			if (conn->getAliveIdleTime() > maxIdleTime_ * 1000)
			{
				conns_.pop();
				connSize_--;
				delete conn;
			}
			else
			{
				break;  //队头没超过 其他也没超过
			}
		}
	}
}