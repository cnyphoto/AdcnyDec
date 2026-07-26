#include "pch.h"
#include "DbToCode.h"
#include "SqlStore.h"
#include <iostream>

DbToCode::DbToCode()
{
}

DbToCode::~DbToCode()
{
}

void DbToCode::setConstr(std::string sqlip, int port, std::string user, std::string psd, std::string dbName)
{
	this->constr.sqlip = sqlip;
	this->constr.port = port;
	this->constr.dbName = dbName;
	this->constr.psd = psd;
	this->constr.user = user;
}

SorcePar DbToCode::getPar(int scoild) const
{
	SqlStore conn;
	conn.connect(this->constr.sqlip, this->constr.port, this->constr.user, this->constr.psd, this->constr.dbName);
	char sql[1024] = { 0 };
	sprintf_s(sql, "select * from visonpar WHERE scoilid=%d", scoild);
	auto res = conn.query(sql);

	unsigned int num_fields;
	num_fields = mysql_num_fields(res);
	MYSQL_ROW row;

	SorcePar spar;
	while ((row = mysql_fetch_row(res))) {
		spar.id= std::stoi(row[0]);
		spar.scoilid = std::stoi(row[1]);
		spar.imgRoot = row[3];
		spar.filepolling = std::stoi(row[5]);
		spar.fileType = row[6];
		spar.savePath = row[7];
		spar.ascNum = std::stoi(row[8]);
		spar.openAied = std::stoi(row[9]);
		spar.imgW = std::stoi(row[10]);
		spar.imgH = std::stoi(row[11]);
		spar.du = std::stoi(row[12]);
		spar.rl = std::stoi(row[13]);
		spar.Acquisition = std::stoi(row[15]);
		spar.colorNum = std::stoi(row[16]);
		spar.webSockectIpPort = row[17];
		spar.closed = atoi(row[18]);
		spar.dataEned= atoi(row[24]);
		spar.tested = atoi(row[25]);
		spar.trained= atoi(row[26]);
		spar.CamId = row[28];
	}
	return spar;
}

std::vector<modpar> DbToCode::getMod(int sparid) const
{
	SqlStore conn;
	conn.connect(this->constr.sqlip, this->constr.port, this->constr.user, this->constr.psd, this->constr.dbName);
	char sql[1024] = { 0 };
	sprintf_s(sql, "SELECT * FROM `aimod` WHERE visonParId=%d  ORDER BY `name`", sparid);
	auto res = conn.query(sql);

	unsigned int num_fields;
	num_fields = mysql_num_fields(res);
	MYSQL_ROW row;

	std::vector<modpar> mods;

	while ((row = mysql_fetch_row(res))) {

		modpar mdp;

		mdp.id = std::stoi(row[0]);
		mdp.name = std::stoi(row[1]);
		mdp.modpath = row[2];
		mdp.slot = std::stoi(row[4]);
		mdp.conf = std::stof(row[5]);
		mdp.modh= std::stoi(row[7]);
		mdp.modw = std::stoi(row[8]);
		mods.push_back(mdp);
	}
	return mods;
}

void DbToCode::insetData(std::string sql) const
{
	/*SqlStore conn;
	conn.connect(this->constr.sqlip, this->constr.port, this->constr.user, this->constr.psd, this->constr.dbName);
	conn.update(sql);*/

	SqlStore* conn=new SqlStore();

	// 1. 连接数据库（判断是否成功）
	bool isConnect = conn->connect(
		this->constr.sqlip,
		this->constr.port,
		this->constr.user,
		this->constr.psd,
		this->constr.dbName
	);

	if (!isConnect)
	{
		std::cerr << "DB connect failed!" << std::endl;
		return;
	}

	try
	{
		// 2. 执行 SQL（insert/update/delete 通用）
		bool ret = conn->update(sql);
		if (!ret)
		{
			std::cerr << "SQL exec failed: " << sql << std::endl;
		}
	}
	catch (...)
	{
		std::cerr << "SQL exec exception!" << std::endl;
	}

	// 3. close connection to prevent leak
	delete conn;
	conn = nullptr;	
}

void DbToCode::combDecData(std::vector<std::vector<int>> bboxs,long long coild,int scoilid)
{
	size_t num = bboxs.size();
	for (int i = 2; i < num; i++)
	{
		char sql[1024] = { 0 };
		sprintf_s(sql, this->sqlStrBses[1].c_str(),0, bboxs[i][0], bboxs[i][1], bboxs[i][2], bboxs[i][3], bboxs[i][4], bboxs[i][5], bboxs[i][6], bboxs[i][7], coild, scoilid);
		this->insetData(sql);
	}
}

void DbToCode::toUpDataImgNum(const int &n,const long long &colid)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, this->sqlStrBses[3].c_str(), n, colid);
	this->insetData(sql);
}

long long DbToCode::combCoilData(bool upCoilum,int arc, char* argv[])
{
	if (upCoilum)
	{
		long long coilid;
		sscanf_s(argv[1], "%lld", &coilid);
		//auto aa = argv[2];
		if (arc > 3)
		{
			char sql[1024] = { 0 };
			sprintf_s(sql, this->sqlStrBses[0].c_str(), coilid, argv[2], argv[3]);
			std::cout << sql << std::endl;
			this->insetData(sql);
		}

		return coilid;
	}
	return 0;
}

void DbToCode::comCalData(std::vector<std::vector<int>> bboxs,int n)
{
	size_t num= bboxs.size();
	char sql[1024] = { 0 };
	sprintf_s(sql, this->sqlStrBses[2].c_str(), num > 2 ? bboxs[2][5] : -1, bboxs[0][0], bboxs[1][0], n);
	this->insetData(sql);
}




