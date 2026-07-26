#pragma once
#include <ImgCal.h>
#include "EdgCalTcm.h"


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
	int imgValid(cv::Mat mat);
	cv::Mat setDst(cv::Mat img);

	std::vector<std::vector<int>> getBboxs(int num);
};

