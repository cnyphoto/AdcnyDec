#pragma once
#include <windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <ImgCal.h>
#include <EdgeCa.h>

using namespace std;
using namespace cv;


#ifdef ADCNYEDG_EXPORTS
#define ADCNYEDG_API __declspec(dllexport)
#else
#define ADCNYEDG_API __declspec(dllimport)
#endif

class __declspec(dllexport) EdgeSin : public ImgCal
{
public:
	EdgeSin();
	~EdgeSin();
	int imgValid(cv::Mat mat) { return 0; };
	cv::Mat setDst(cv::Mat img) { return this->img; };

	std::vector<std::vector<int>> getBboxs(int num);
private:
	EdgeCa* edgeCa;
};
