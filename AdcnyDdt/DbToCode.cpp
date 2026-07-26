#include "pch.h"
#include "DbToCode.h"
#include "SqlStore.h"
#include <iostream>
#include <fstream>
#include <sstream>

DbToCode::DbToCode()
	: dbConn(nullptr)
{
	loadFromIni();
}

DbToCode::~DbToCode()
{
	if (dbConn) {
		delete dbConn;
		dbConn = nullptr;
	}
}

void DbToCode::loadFromIni(const std::string& iniPath)
{
	std::ifstream file(iniPath);
	if (!file.is_open()) {
		std::cerr << "DbToCode: 无法打开 " << iniPath << std::endl;
		return;
	}

	std::string line, section;
	while (std::getline(file, line)) {
		if (line.empty() || line[0] == ';' || line[0] == '#')
			continue;
		if (line[0] == '[') {
			section = line.substr(1, line.find(']') - 1);
			continue;
		}

		std::string key, value;
		std::stringstream ss(line);
		std::getline(ss, key, '=');
		std::getline(ss, value);

		if (section == "DbConnection") {
			if (key == "ip")       this->constr.sqlip  = value;
			if (key == "port")     this->constr.port   = std::stoi(value);
			if (key == "user")     this->constr.user   = value;
			if (key == "password") this->constr.psd    = value;
			if (key == "dbName")   this->constr.dbName = value;
		}
		else if (section == "Camera") {
			if (key == "type")     this->softCfg.cameraType = value;
		}
		else if (section == "Detector") {
			if (key == "type")     this->softCfg.detectorType = value;
		}
		else if (section == "Database") {
			if (key == "saveDb")   this->softCfg.saveDb = (value == "1");
			if (key == "scoilid")  this->softCfg.scoilid = std::stoi(value);
		}
		else if (section == "Alert") {
			if (key == "sendAlert") this->softCfg.sendAlert = (value == "1");
		}
		else if (section == "Scoilid") {
			if (key == "scoilid")  this->softCfg.scoilid = std::stoi(value);
		}
	}
}

void DbToCode::setConstr(std::string sqlip, int port, std::string user, std::string psd, std::string dbName)
{
	this->constr.sqlip = sqlip;
	this->constr.port = port;
	this->constr.dbName = dbName;
	this->constr.psd = psd;
	this->constr.user = user;
}

SorcePar DbToCode::getPar() const
{
	SqlStore conn;
	conn.connect(this->constr.sqlip, this->constr.port, this->constr.user, this->constr.psd, this->constr.dbName);
	char sql[1024] = { 0 };
	sprintf_s(sql, "select * from visonpar WHERE scoilid=%d", softCfg.scoilid);
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
	std::lock_guard<std::mutex> lock(dbMtx);

	// 持久连接：未创建或断线时自动重连
	if (!dbConn || !dbConn->ping())
	{
		if (dbConn) {
			delete dbConn;
		}
		dbConn = new SqlStore();
		if (!dbConn->connect(
			this->constr.sqlip,
			this->constr.port,
			this->constr.user,
			this->constr.psd,
			this->constr.dbName))
		{
			std::cerr << "DB connect failed!" << std::endl;
			return;
		}
	}

	try
	{
		bool ret = dbConn->update(sql);
		if (!ret)
		{
			std::cerr << "SQL exec failed: " << sql << std::endl;
		}
	}
	catch (...)
	{
		std::cerr << "SQL exec exception!" << std::endl;
	}
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

void DbToCode::combDecValStr(const std::string& valStr) const
{
	char sql[1024] = { 0 };
	int v[9];
	long long v9;
	int v10;
	sscanf_s(valStr.c_str(), "%d,%d,%d,%d,%d,%d,%d,%d,%d,%lld,%d",
		&v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], &v9, &v10);
	sprintf_s(sql, this->sqlStrBses[1].c_str(),
		v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v9, v10);
	this->insetData(sql);
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




