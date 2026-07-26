#pragma once
#include <ImgCal.h>
#include <EdgeCalSilk.h>

#ifdef ADCNYVDS_EXPORTS
#define ADCNYVDS_API __declspec(dllexport)
#else
#define ADCNYVDS_API __declspec(dllimport)
#endif


class __declspec(dllexport) FlySilkDec:public ImgCal
{
public:
	FlySilkDec();
	~FlySilkDec();

	std::vector<int> calSize(cv::Mat calMat) { return { 0 }; };
	int imgValid(cv::Mat mat);
	cv::Mat setDst(cv::Mat img);

	std::vector<std::vector<int>> getBboxs(int num);
private:
	EdgeCalSilk* edgeCal;
	std::map<std::string, int> config;
};

