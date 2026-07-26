#pragma once
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <imgOp.h>

#ifdef ADCNYBSE_EXPORTS
#define ADCNYBSE_API __declspec(dllexport)
#else
#define ADCNYBSE_API __declspec(dllimport)
#endif

class ADCNYBSE_API ImgCal
{
public:
	ImgCal();
	virtual ~ImgCal();
	virtual int imgValid(cv::Mat mat)=0;
	virtual cv::Mat setDst(cv::Mat img)=0;
	virtual std::vector<std::vector<int>> getBboxs(int num)=0;

	cv::Mat img;
	int colorNum;
	int imgW;
	int imgH;
	float edgeRatio;
	int edge;
	long long coilnum;
	int scoilid;
	int rl;
	int du;
	std::unique_ptr<imgOp> imgop;
};
