#pragma once
#include <ImgCal.h>


#ifdef ADCNYFAM_EXPORTS
#define ADCNYFAM_API __declspec(dllexport)
#else
#define ADCNYFAM_API __declspec(dllimport)
#endif


class __declspec(dllexport) WholeDec : public ImgCal
{
public:
	WholeDec();
	~WholeDec();

	std::vector<int> calSize(cv::Mat calMat) { return { 0 }; };
	std::vector<int> getEdge(cv::Mat mat1, bool lefted, int ft);
	int imgValid(cv::Mat mat);
	cv::Mat setDst(cv::Mat img);

	std::vector<std::vector<int>> getBboxs(int num);
};

