#include "Connection.h"
#include <iostream>
#include <time.h>
#include "CommonConnectionPool.h"
#include <thread>


int main()
{
	/*Connection conn;
	char sql[1024] = { 0 };
	sprintf_s(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
		"zhang san", 20, "male");
	conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
	conn.update(sql);*/
	ConnectionPool* conns = conns->getConnectionPool();
	
	std::clock_t start = std::clock();
	std::thread t1([&]() {
		for (int i = 0; i < 2500; i++)
		{
			std::shared_ptr<Connection> conn = conns->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"MEME", 20, "male");
			conn->update(sql);
		}
	});
	std::thread t2([&]() {
		for (int i = 0; i < 2500; i++)
		{
			std::shared_ptr<Connection> conn = conns->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name, age, sex) values('%s','%d','%s')",
				"GANDAMU", 20, "male");
			conn->update(sql);
		}
	});
	std::thread t3([&]() {
		for (int i = 0; i < 2500; i++)
		{
			std::shared_ptr<Connection> conn = conns->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name, age, sex) values('%s','%d','%s')",
				"BATMAN", 20, "male");
			conn->update(sql);
		}
	});
	std::thread t4([&]() {
		for (int i = 0; i < 2500; i++)
		{
			std::shared_ptr<Connection> conn = conns->getConnection();
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name, age, sex) values('%s','%d','%s')",
				"MK", 20, "female");
			conn->update(sql);
		}
	});
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	std::clock_t end = std::clock();

	std::cout << end - start << "ms" << std::endl;
	return 0;
}