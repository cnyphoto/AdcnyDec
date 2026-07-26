#pragma once
#include <string>
#include <mysql.h>
#include <chrono>

class  SqlStore
{
public:
	SqlStore();
	~SqlStore();
	bool connect(const std::string& ip,
		const uint16_t port,
		const std::string& user,
		const std::string& pwd,
		const std::string& dbName);
	// 更新操作 insert、delete、update
	bool update(const std::string& sql);
	// 查询操作 select
	MYSQL_RES* query(const std::string& sql);
	//刷新存活时间
	void refreshAliveTime()
	{
		aliveTime = std::chrono::steady_clock::now();
	}
	//获取存活时间间隔
	size_t getAliveTime()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - aliveTime).count();
	}
private:
	MYSQL* conn;//数据库连接
	std::chrono::time_point<std::chrono::steady_clock> aliveTime; //连接存活时间
};


