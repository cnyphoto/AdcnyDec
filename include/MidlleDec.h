#pragma once
#include <ImgCal.h>


#ifdef ADCNYMID_EXPORTS
#define ADCNYMID_API __declspec(dllexport)
#else
#define ADCNYMID_API __declspec(dllimport)
#endif

class __declspec(dllexport) MidlleDec: public ImgCal
{
public:
	MidlleDec();
	~MidlleDec();

	std::vector<int> calSize(cv::Mat calMat) { return { 0 }; };
	int imgValid(cv::Mat mat);
	cv::Mat setDst(cv::Mat img);

	std::vector<std::vector<int>> getBboxs(int num);
};

