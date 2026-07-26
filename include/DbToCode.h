#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <queue>

class SqlStore;
#include <InferStore.h>
#include <Target.h>


#ifdef ADCNYDDT_EXPORTS
#define ADCNYDDT_API __declspec(dllexport)
#else
#define ADCNYDDT_API __declspec(dllimport)
#endif

class __declspec(dllexport) DbToCode
{
public:
	DbToCode();
	~DbToCode();

	typedef struct ConStr {
		std::string sqlip;
		int port;
		std::string user;
		std::string psd;
		std::string dbName;
	}ConStr;
	void setConstr(std::string sqlip, int port, std::string user, std::string psd, std::string dbName);
	SorcePar getPar(int scoild) const;
	std::vector<modpar> getMod(int sparid) const;
	void insetData(const std::string sql) const;
	void combDecData(std::vector<std::vector<int>> bbox, long long coild, int scoilid);
	void combDecValStr(const std::string& valStr) const;
	void toUpDataImgNum(const int& n, const long long& colid);
	long long combCoilData(bool upCoilum, int arc, char* argv[]);
	void comCalData(std::vector<std::vector<int>> bboxs, int n);
	void startInfer(std::vector<modpar> mods,std::string flord);
	void imgInferToDb(std::vector<std::string> sqls) const;
	void toInfer(const cv::Mat img, std::vector<std::vector<int>> bbox, long long coild, int modtype, int imgNum, int saveType);
	const std::string& getDecInsertSqlTemplate() const { return sqlStrBses[1]; }

private:
	ConStr constr;
	const std::vector<std::string> sqlStrBses{ "insert into `coilstore`(CoilNum,CoilWidth,CoilThickness) values(%lld,%s,%s)",
		"insert into `dec`(imgdecid,area,leftt,top,width,height,imgid,edge,decid,coilid,scoilid) values(%d,%d,%d,%d,%d,%d,%d,%d,%d,%lld,%d)",
	"UPDATE edgecal SET imgid=%d,colorval=%d,edge=%d WHERE id=%d",
	"UPDATE coilstore SET EndImgId = %d WHERE CoilNum = %lld" };
	mutable SqlStore* dbConn;
	mutable std::mutex dbMtx;
	InferStore* inferstore;
	std::mutex mtx;
};

