#include "pch.h"
#include "SqlStore.h"

SqlStore::SqlStore()
{
	conn = mysql_init(nullptr);
	auto s = mysql_options(conn,
		MYSQL_SET_CHARSET_NAME,
		MYSQL_AUTODETECT_CHARSET_NAME);
	/*mysql_query(conn, "set names gbk");*/
	/*mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8");*/
	mysql_set_character_set(conn, "gbk");
}

SqlStore::~SqlStore()
{
	if (conn != nullptr)
	{
		mysql_close(conn);
	}
}

bool SqlStore::connect(const std::string& ip, const uint16_t port, const std::string& user, const std::string& pwd, const std::string& dbName)
{
	MYSQL* ptr = mysql_real_connect(conn, ip.c_str(), user.c_str(), pwd.c_str(), dbName.c_str(), port, nullptr, 0);
	return ptr != nullptr;
}

bool SqlStore::ping()
{
	return mysql_ping(conn) == 0;
}

bool SqlStore::update(const std::string& sql)
{
	if (mysql_query(conn, sql.c_str()))
	{
		return false;
	}
	return true;
}

MYSQL_RES* SqlStore::query(const std::string& sql)
{
	auto sss = sql.c_str();
	if (mysql_query(conn, sss))
	{
		return nullptr;
	}
	return mysql_use_result(conn);
}
