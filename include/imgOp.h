#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>

#include <fstream>
#include <sstream>
#include <map>

#ifdef ADCNYBSE_EXPORTS
#define ADCNYBSE_API __declspec(dllexport)
#else
#define ADCNYBSE_API __declspec(dllimport)
#endif

class __declspec(dllexport) imgOp
{
public:
	imgOp(std::string folder);
	~imgOp();
	void saveImg(int num, cv::Mat mat, long long coilnum,int scoild,int detected);
	cv::Mat enhancedPhotoForMultiply(cv::Mat mat);
	cv::Mat contrastRatio(cv::Mat mat);
	std::map<std::string, int> readCalINI(const std::string& filename);
private:
	std::string folder;
};

